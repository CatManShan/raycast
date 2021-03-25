#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem-utils/mem-macros.h"
#include "raycast-engine/raycast-engine.h"
#include "simpcg/simpcg.h"

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

static void map_init(struct REMap *map);
static void draw_frame(struct REMap *map, struct SCGBuffer *pixel_buffer, double origin_x, double origin_y, double forward_angle);
static void angle_to_vector(double angle, double length, double *vec_x, double *vec_y);
static double coords_to_angle(double x, double y);
static double reduce_angle(double angle);
static int32_t min_int32(int32_t a, int32_t b); 

int main()
{
#ifdef MEM_DEBUG
	FILE *debug_file = fopen("mem-debug.log", "w");
	debug_set_out_stream(debug_file);
	debug_start();
#endif // MEM_DEBUG

	struct REMap *map = re_map_create(32, 24);
	map_init(map);
	// map_print(map);
	printf("\n");

	struct SCGBuffer *pixel_buffer = scg_pixel_buffer_create(118, 69);
	scg_pixel_buffer_make_space(pixel_buffer);
	scg_input_adjust();

	struct Player {
		double pos_x;
		double pos_y;
		double rotation;
	} player = { 8, 8, PI / 2 };

	bool quit = false;
	while (!quit) {
		draw_frame(map, pixel_buffer, player.pos_x, player.pos_y, player.rotation);
		usleep(1000000 / 60);

		const double PLAYER_SPEED = 0.25;
		const double PLAYER_TURN_SPEED = PI / 64;
		double move_x, move_y;
		switch (getchar()) {
		case CTRL_C:
			quit = true;
			break;
		case 'w':
			angle_to_vector(player.rotation, PLAYER_SPEED, &move_x, &move_y);
			player.pos_x += move_x;
			player.pos_y += move_y;
			break;
		case 's':
			angle_to_vector(player.rotation + PI, PLAYER_SPEED, &move_x, &move_y);
			player.pos_x += move_x;
			player.pos_y += move_y;
			break;
		case 'a':
			angle_to_vector(player.rotation + PI / 2, PLAYER_SPEED, &move_x, &move_y);
			player.pos_x += move_x;
			player.pos_y += move_y;
			break;
		case 'd':
			angle_to_vector(player.rotation - PI / 2, PLAYER_SPEED, &move_x, &move_y);
			player.pos_x += move_x;
			player.pos_y += move_y;
			break;
		case 'j':
			player.rotation += PLAYER_TURN_SPEED;
			break;
		case 'l':
			player.rotation -= PLAYER_TURN_SPEED;
			break;
		default:
			break;
		}
	}

	scg_pixel_buffer_remove_space(pixel_buffer);
	scg_pixel_buffer_destroy(pixel_buffer);
	scg_input_restore();

	re_map_destroy(map);

#ifdef MEM_DEBUG
	fprintf(debug_file, "Unfreed pointers:\n");
	debug_print_allocated();
	fclose(debug_file);
	debug_end();
#endif // MEM_DEBUG

	return EXIT_SUCCESS;
}

static void map_init(struct REMap *map)
{
	re_map_fill(map, RE_MAP_CELL_SOLID(FLOOR));

	struct REMapCell blue_wall_cell = (struct REMapCell) { BLUE_WALL, BRIGHT_BLUE_WALL, BLUE_WALL, BRIGHT_BLUE_WALL };
	for (size_t x = 4; x < 12; x++) {
		re_map_set_cell(map, x, 11, blue_wall_cell);
	}
	for (size_t y = 9; y < 11; y++) {
		re_map_set_cell(map, 4, y, blue_wall_cell);
	}

	struct REMapCell funky_cell = (struct REMapCell) { RED_WALL, OUT_OF_BOUNDS, FLOOR, YELLOW_WALL };
	re_map_set_cell(map, 14, 3, funky_cell);
}

static void draw_frame(struct REMap *map, struct SCGBuffer *pixel_buffer, double origin_x, double origin_y, double forward_angle)
{
	int32_t screen_width = pixel_buffer->width / 2;
	int32_t screen_height = pixel_buffer->height;

	for (int32_t row = 0; row < screen_height; row++) {
		for (int32_t col = 0; col < screen_width; col++) {
			scg_pixel_buffer_set(pixel_buffer, col, row, FLOOR);
		}
	}

	int32_t scaler_dimension = min_int32(screen_width, screen_height);

	double lengths[screen_width];
	enum Material materials[screen_width];

	// Calculate values
	for (int32_t line = 0; line < screen_width; line++)
	{
		double line_center_offset = (int32_t) line - (int32_t) screen_width / 2;
		double rel_angle = -coords_to_angle(1, line_center_offset / (scaler_dimension));

		enum Material collided_material;
		double forward_distance = re_raycast(map, origin_x, origin_y, forward_angle, rel_angle, FLOOR, OUT_OF_BOUNDS, &collided_material);

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
		if (end >= screen_height) {
			end = screen_height - 1;
		}

		enum Material material = materials[line];
		for (int32_t row = start; row < end; row++) {
			scg_pixel_buffer_set(pixel_buffer, line, row, material);
		}
	}

	// Print
	scg_pixel_buffer_print(pixel_buffer);
}

static void angle_to_vector(double angle, double length, double *vec_x, double *vec_y)
{
	angle = reduce_angle(angle);
	
	if (length == 0)
	{
		*vec_x = 0;
		*vec_y = 0;

		return;
	}
	
	double slope = tan(angle);
	
	*vec_x = 1;
	*vec_y = slope;
	
	double scale = sqrt(*vec_x * *vec_x + *vec_y * *vec_y);
	
	*vec_x *= length / scale;
	*vec_y *= length / scale;
	
	if (angle > PI / 2 && angle <= 3 * PI / 2)
	{
		*vec_x *= -1;
		*vec_y *= -1;
	}
}

static double coords_to_angle(double x, double y)
{
	return atan2(y, x);
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

