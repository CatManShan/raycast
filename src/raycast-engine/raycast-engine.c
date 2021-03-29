#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "../mem-utils/mem-macros.h"

#ifdef MEM_DEBUG
#include "../mem-utils/mem-debug.h"
#endif // MEM_DEBUG

#include "raycast-engine.h"

#define THRESHOLD 0x0.0001p0
#define PI 3.14159265358979323846

static double reduce_angle(double angle);
static double distance_of_points(double x1, double y1, double x2, double y2);
static bool double_less_than_or_equal(double a, double b);
static bool double_equal(double a, double b);

struct REMap *re_map_create(uint32_t width, uint32_t height)
{
	uint64_t area = (uint64_t) width * height;
	struct REMap *map = ALLOC_FLEX_STRUCT(map, cells, area);

	map->width = width;
	map->height = height;

	return map;
}

void re_map_destroy(struct REMap *map)
{
	free(map);
}

struct REMapCell re_map_get_cell(struct REMap *map, uint32_t x, uint32_t y)
{
	return map->cells[y * map->width + x];
}

void re_map_set_cell(struct REMap *map, uint32_t x, uint32_t y, struct REMapCell cell)
{
	map->cells[y * map->width + x] = cell;
}

void re_map_fill(struct REMap *map, struct REMapCell cell)
{
	uint64_t area = (uint64_t) map->width * map->height;
	for (uint64_t index = 0; index < area; index++) {
		map->cells[index] = cell;
	}
}

/* NOTE: only works for angles >=  0 and < 2PI */
static uint8_t get_angle_quadrant(double reduced_angle)
{
	return (uint8_t) (reduced_angle / (PI / 2) + 1);
}

double re_raycast(struct REMap *map, double origin_x, double origin_y, double forward_angle, double rel_angle,
		int transparent_material, int out_of_bounds_material, int *collided_material)
{
	int32_t origin_x_whole = (int32_t) origin_x;
	int32_t origin_y_whole = (int32_t) origin_y;
	double origin_x_frac = origin_x - origin_x_whole;
	double origin_y_frac = origin_y - origin_y_whole;
	double absolute_angle = reduce_angle(forward_angle + rel_angle);

	uint8_t quadrant = get_angle_quadrant(absolute_angle);

	double x_intercept = origin_x + (1 - origin_y_frac) / tan(absolute_angle);
	double y_intercept = origin_y + (1 - origin_x_frac) * tan(absolute_angle);

	int32_t check_x = origin_x_whole + 1;
	int32_t check_y = origin_y_whole + 1;

	double step_x = 1 / tan(absolute_angle);
	double step_y = tan(absolute_angle);

	int32_t tile_step_x = 1;
	int32_t tile_step_y = 1;

	if (quadrant == 2 || quadrant == 3)
	{
		tile_step_x = -1;
		step_y *= -1;

		check_x += tile_step_x;
		y_intercept += step_y;
	}
	if (quadrant == 3 || quadrant == 4)
	{
		tile_step_y = -1;
		step_x *= -1;

		check_y += tile_step_y;
		x_intercept += step_x;
	}

	bool found_horiz_wall = false;
	bool found_vert_wall = false;

	double collision_coords[2] = {0, 0};
	while (!found_horiz_wall && !found_vert_wall) {
		while (!found_horiz_wall && double_less_than_or_equal(tile_step_x * x_intercept, tile_step_x * check_x)) {
			int32_t x_intercept_floor = (int32_t) x_intercept;

			int top_cell_material    = re_map_coords_in_bounds(map, x_intercept_floor, check_y)
				? re_map_get_cell(map, x_intercept_floor, check_y).material_bottom
				: out_of_bounds_material;

			int bottom_cell_material = re_map_coords_in_bounds(map, x_intercept_floor, check_y - 1)
				? re_map_get_cell(map, x_intercept_floor, check_y - 1).material_top
				: out_of_bounds_material;

			int materials[2];
			if (quadrant == 1 || quadrant == 2) {
				materials[0] = bottom_cell_material;
				materials[1] = top_cell_material;
			} else {
				materials[0] = top_cell_material;
				materials[1] = bottom_cell_material;
			}

			if (materials[0] != transparent_material) {
				*collided_material = materials[0];
				found_horiz_wall = true;
			} else if (materials[1] != transparent_material) {
				*collided_material = materials[1];
				found_horiz_wall = true;
			}

			if (found_horiz_wall) {
				collision_coords[0] = x_intercept;
				collision_coords[1] = check_y;
			} else { // Step
				check_y += tile_step_y;
				x_intercept += step_x;
			}
		}

		while (!found_horiz_wall && !found_vert_wall && double_less_than_or_equal(tile_step_y * y_intercept, tile_step_y * check_y))
		{
			int32_t y_intercept_floor = (int32_t) y_intercept;
			int right_cell_material = re_map_coords_in_bounds(map, check_x, y_intercept_floor)
				? re_map_get_cell(map, check_x, y_intercept_floor).material_left
				: out_of_bounds_material;
			int left_cell_material  = re_map_coords_in_bounds(map, check_x - 1, y_intercept_floor)
				? re_map_get_cell(map, check_x - 1, y_intercept_floor).material_right
				: out_of_bounds_material;

			int materials[2];
			if (quadrant == 1 || quadrant == 4) {
				materials[0] = left_cell_material;
				materials[1] = right_cell_material;
			} else {
				materials[0] = right_cell_material;
				materials[1] = left_cell_material;
			}

			if (materials[0] != transparent_material) {
				*collided_material = materials[0];
				found_vert_wall = true;
			} else if (materials[1] != transparent_material) {
				*collided_material = materials[1];
				found_vert_wall = true;
			}

			if (found_vert_wall) {
				collision_coords[0] = check_x;
				collision_coords[1] = y_intercept;
			} else { // Step
				check_x += tile_step_x;
				y_intercept += step_y;
			}
		}
	}

	double travel_distance = distance_of_points(origin_x, origin_y, collision_coords[0], collision_coords[1]);
	double forward_distance = travel_distance * cos(rel_angle);

	return forward_distance;
}

bool re_map_coords_in_bounds(struct REMap *map, int64_t x, int64_t y)
{
	return (x >= 0 && y >= 0 && x < map->width && y < map->height);
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

static double distance_of_points(double x1, double y1, double x2, double y2)
{
	double dx = x2 - x1;
	double dy = y2 - y1;

	return sqrt(dx * dx + dy * dy);
}

static bool double_less_than_or_equal(double a, double b)
{
	return a <= b || double_equal(a, b);
}

static bool double_equal(double a, double b)
{
	return fabs(a - b) < THRESHOLD;
}


