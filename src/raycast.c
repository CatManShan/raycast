#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem-utils/mem-macros.h"
#include "option-map/option-map.h"
#include "raycast-engine/raycast-engine.h"
#include "simptg/simptg.h"
#include "maze-gen/maze-gen.h"

#ifdef MEM_DEBUG
#include "mem-utils/mem-debug.h"
#endif

#define PI 3.14159265358979323846
#define CTRL_C '\003'

enum Material {
	OUT_OF_BOUNDS = SCG_COLOR_BRIGHT_BLACK,
	FLOOR = SCG_COLOR_BLACK,
	BLUE_WALL = SCG_COLOR_BLUE,
	BRIGHT_BLUE_WALL = SCG_COLOR_BRIGHT_BLUE,
	RED_WALL = SCG_COLOR_RED,
	YELLOW_WALL = SCG_COLOR_YELLOW
};

struct Options {
	uint16_t width;
	uint16_t height;
};

struct Player {
	double x;
	double y;
	double rotation;
};

struct CrossThreadData {
	volatile struct Player *p_player;
	struct REMap *map;
	volatile bool quit;
};

static struct Options parse_options(int argc, char **argv);
static void init_map(struct REMap *map);
static void draw_frame(struct REMap *map, struct SCGBuffer *pixel_buffer, double origin_x, double origin_y, double forward_angle);
static void angle_to_vector(double angle, double length, double *vec_x, double *vec_y);
static double vector_to_angle(double x, double y);
static double reduce_angle(double angle);
static int32_t min_int32(int32_t a, int32_t b); 
static int32_t move_player(volatile struct Player *p_player, double dx, double dy, struct REMap *map);

void *input_loop_func(void *vp_data);

int main(int argc, char **argv)
{
#ifdef MEM_DEBUG
	FILE *debug_file = fopen("mem-debug.log", "w");
	debug_set_out_stream(debug_file);
	debug_start();
#endif // MEM_DEBUG
	
	struct Options options = parse_options(argc - 1, &argv[1]);

	struct REMap *map = re_map_create(32, 32);
	init_map(map);
	printf("\n");

	struct SCGBuffer *pixel_buffer = stg_pixel_buffer_create(options.width, options.height);
	stg_pixel_buffer_make_space(pixel_buffer);
	stg_input_adjust();

	volatile struct Player player = { 0.5, map->height - 0.5, 0.0625 };

	struct CrossThreadData data = { .p_player = &player, .map = map, .quit = false };
	pthread_t input_thread;
	pthread_create(&input_thread, NULL, input_loop_func, &data);

	while (!data.quit) {
		draw_frame(map, pixel_buffer, player.x, player.y, player.rotation);
		usleep(1000000 / 60);
	}

	stg_pixel_buffer_remove_space(pixel_buffer);
	stg_pixel_buffer_destroy(pixel_buffer);
	stg_input_restore();

	re_map_destroy(map);

#ifdef MEM_DEBUG
	fprintf(debug_file, "Unfreed pointers:\n");
	debug_print_allocated();
	fclose(debug_file);
	debug_end();
#endif // MEM_DEBUG

	return EXIT_SUCCESS;
}

static struct Options parse_options(int argc, char **argv)
{
	char *size_aliases[] = { "--size", "-s", NULL };

	struct OptionMapOption option_arr[] = {
		{ .aliases = size_aliases, .takes_value = true }
	};
	size_t option_count = 1;

	struct OptionMap *option_map = option_map_create(option_arr, option_count);
	struct OptionMapError error = option_map_set_options(option_map, argc, argv);

	if (error.error_code != OM_NO_ERROR) {
		option_map_print_error_message(stderr, "raycast: ", error);

		exit(EXIT_FAILURE);
	}

	struct Options options = { .width = 64, .height = 48 };

	if (option_map_is_option_given(option_map, "--size")) {
		char *size = option_map_get_option_value(option_map, "--size");
		sscanf(size, "%hux%hu", &options.width, &options.height);
	}

	option_map_destroy(option_map);

	return options;
}

static void init_map(struct REMap *map)
{
	re_map_fill(map, RE_MAP_CELL_SOLID(FLOOR));

	struct Maze *maze = maze_create(map->width, map->height);
	maze_generate(maze);

	for (uint32_t row = 0; row < maze->height; row++) {
		for (uint32_t col = 0; col < maze->width; col++) {
			struct REMapCell cell = RE_MAP_CELL_SOLID(FLOOR);

			if (maze_has_wall(maze, col, row, MAZE_WALL_TOP)) {
				cell.material_top = BRIGHT_BLUE_WALL;
			}
			if (maze_has_wall(maze, col, row, MAZE_WALL_RIGHT)) {
				cell.material_right = BLUE_WALL;
			}
			if (maze_has_wall(maze, col, row, MAZE_WALL_BOTTOM)) {
				cell.material_bottom = BRIGHT_BLUE_WALL;
			}
			if (maze_has_wall(maze, col, row, MAZE_WALL_LEFT)) {
				cell.material_left = BLUE_WALL;
			}

			re_map_set_cell(map, col, (map->height - 1) - row, cell);
		}
	}

	maze_destroy(maze);
}

static void draw_frame(struct REMap *map, struct SCGBuffer *pixel_buffer, double origin_x, double origin_y, double forward_angle)
{
	int32_t screen_width = pixel_buffer->width / 2;
	int32_t screen_height = pixel_buffer->height;

	for (int32_t row = 0; row < screen_height; row++) {
		for (int32_t col = 0; col < screen_width; col++) {
			stg_pixel_buffer_set(pixel_buffer, col, row, FLOOR);
		}
	}

	int32_t scaler_dimension = min_int32(screen_width, screen_height);

	double lengths[screen_width];
	enum Material materials[screen_width];

	// Calculate values
	for (int32_t line = 0; line < screen_width; line++)
	{
		double line_center_offset = (int32_t) line - (int32_t) screen_width / 2;
		double rel_angle = -vector_to_angle(1, line_center_offset / (scaler_dimension));

		enum Material collided_material;
		double forward_distance = re_cast_ray(map, origin_x, origin_y, forward_angle, rel_angle, FLOOR, OUT_OF_BOUNDS, &collided_material);

		double length = scaler_dimension / forward_distance;

		lengths[line] = length;
		materials[line] = collided_material;
	}

	// Draw
	for (int32_t line = 0; line < screen_width; line++)
	{
		int32_t length = (int32_t) round(lengths[line]);
		int32_t start = round((screen_height - length) / 2);
		int32_t end = round((screen_height + length) / 2);

		if (start < 0) {
			start = 0;
		}
		if (end > screen_height) {
			end = screen_height;
		}

		enum Material material = materials[line];
		for (int32_t row = start; row < end; row++) {
			stg_pixel_buffer_set(pixel_buffer, line, row, material);
		}
	}

	// Print
	stg_pixel_buffer_print(pixel_buffer);
}

static void angle_to_vector(double angle, double length, double *vx, double *vy)
{
	angle = reduce_angle(angle);
	
	if (length == 0)
	{
		*vx = 0;
		*vy = 0;

		return;
	}
	
	double slope = tan(angle);
	
	*vx = 1;
	*vy = slope;
	
	double scale = sqrt(*vx * *vx + *vy * *vy);
	
	*vx *= length / scale;
	*vy *= length / scale;
	
	if (angle > PI / 2 && angle <= 3 * PI / 2)
	{
		*vx *= -1;
		*vy *= -1;
	}
}

static double vector_to_angle(double vx, double yx)
{
	return atan2(yx, vx);
}

static double reduce_angle(double angle)
{
	if (angle < 0)
	{
		int wraps = (int) (angle / (2 * PI));
		
		angle -= (wraps - 1) * (2 * PI);
	}
	
	if (angle >= (2 * PI))
	{
		int wraps = (int) (angle / (2 * PI));
		
		angle -= wraps * (2 * PI);
	}
	
	return angle;
}

static int32_t min_int32(int32_t a, int32_t b)
{
	return (a < b) ? a : b;
}

static int32_t move_player(volatile struct Player *p_player, double dx, double dy, struct REMap *map)
{
	bool can_move_x = true;
	bool can_move_y = true;

	double x_new = p_player->x + dx;
	double y_new = p_player->y + dy;

	int64_t cell_x = (int64_t) p_player->x; // no floor--should never be negative
	int64_t cell_y = (int64_t) p_player->y; // ^^^
	int64_t cell_x_new = (int64_t) floor(x_new);
	int64_t cell_y_new = (int64_t) floor(y_new);

	if (cell_x != cell_x_new) {
		if (!re_map_coords_in_bounds(map, cell_x_new, cell_y)) {
			can_move_x = false;
		} else {
			struct REMapCell cell_current = re_map_get_cell(map, cell_x, cell_y);
			struct REMapCell cell_new = re_map_get_cell(map, cell_x_new, cell_y);

			enum Material passed_wall_materials[2];
			if (cell_x_new < cell_x) {
				passed_wall_materials[0] = cell_current.material_left;
				passed_wall_materials[1] = cell_new.material_right;
			} else {
				passed_wall_materials[0] = cell_current.material_right;
				passed_wall_materials[1] = cell_new.material_left;
			}

			if (passed_wall_materials[0] != FLOOR || passed_wall_materials[1] != FLOOR) {
				can_move_x = false;
			}
		}
	}
	if (cell_y != cell_y_new) {
		if (!re_map_coords_in_bounds(map, cell_x, cell_y_new)) {
			can_move_y = false;
		} else {
			struct REMapCell cell_current = re_map_get_cell(map, cell_x, cell_y);
			struct REMapCell cell_new = re_map_get_cell(map, cell_x, cell_y_new);

			enum Material passed_wall_materials[2];
			if (cell_y_new < cell_y) {
				passed_wall_materials[0] = cell_current.material_bottom;
				passed_wall_materials[1] = cell_new.material_top;
			} else {
				passed_wall_materials[0] = cell_current.material_top;
				passed_wall_materials[1] = cell_new.material_bottom;
			}

			if (passed_wall_materials[0] != FLOOR || passed_wall_materials[1] != FLOOR) {
				can_move_y = false;
			}
		}
	}

	if (can_move_x) {
		p_player->x += dx;
	}
	if (can_move_y) {
		p_player->y += dy;
	}

	return can_move_x * 2 + can_move_y;
}

void *input_loop_func(void *vp_data)
{
	struct CrossThreadData *p_data = (struct CrossThreadData *) vp_data;
	volatile struct Player *p_player = p_data->p_player;

	while (!p_data->quit) {
		const double PLAYER_BASE_SPEED = 0.125;
		const double PLAYER_TURN_SPEED = PI / 48;

		double move_x, move_y;

		char input = getchar();
		double speed = islower(input) ? PLAYER_BASE_SPEED : PLAYER_BASE_SPEED * 2;
		switch (tolower(input)) {
		case CTRL_C:
			p_data->quit = true;
			break;
		case 'w':
			angle_to_vector(p_player->rotation, speed, &move_x, &move_y);
			move_player(p_player, move_x, move_y, p_data->map);
			break;
		case 's':
			angle_to_vector(p_player->rotation + PI, speed, &move_x, &move_y);
			move_player(p_player, move_x, move_y, p_data->map);
			break;
		case 'a':
			angle_to_vector(p_player->rotation + PI / 2, speed, &move_x, &move_y);
			move_player(p_player, move_x, move_y, p_data->map);
			break;
		case 'd':
			angle_to_vector(p_player->rotation - PI / 2, speed, &move_x, &move_y);
			move_player(p_player, move_x, move_y, p_data->map);
			break;
		case 'j':
			p_player->rotation += PLAYER_TURN_SPEED;
			break;
		case 'l':
			p_player->rotation -= PLAYER_TURN_SPEED;
			break;
		default:
			break;
		}
	}

	return NULL;
}

