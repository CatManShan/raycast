#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "../fixed/fixed.h"
#include "../mem-utils/mem-macros.h"

#ifdef MEM_DEBUG
#include "../mem-utils/mem-debug.h"
#endif // MEM_DEBUG

#include "raycast-engine.h"

#define PI 3.14159265358979323846

typedef struct Fixed64 fixed64_t;

static double reduce_angle(double angle);
static double distance_of_points(double x1, double y1, double x2, double y2);

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

double re_cast_ray(struct REMap *map, double origin_x, double origin_y, double forward_angle, double rel_angle,
		int transparent_material, int out_of_bounds_material, int *collided_material)
{
	int32_t origin_x_whole = (int32_t) origin_x; // no floor--should always be positive
	int32_t origin_y_whole = (int32_t) origin_y; // ^^^
	double origin_x_frac = origin_x - origin_x_whole;
	double origin_y_frac = origin_y - origin_y_whole;
	double absolute_angle = reduce_angle(forward_angle + rel_angle);

	uint8_t quadrant = get_angle_quadrant(absolute_angle);

	fixed64_t intercept_x = fixed64_from_double(origin_x + (1 - origin_y_frac) / tan(absolute_angle));
	fixed64_t intercept_y = fixed64_from_double(origin_y + (1 - origin_x_frac) * tan(absolute_angle));

	int32_t tile_x = origin_x_whole + 1;
	int32_t tile_y = origin_y_whole + 1;

	fixed64_t step_x = fixed64_from_double(1 / tan(absolute_angle));
	fixed64_t step_y = fixed64_from_double(tan(absolute_angle));

	int32_t tile_step_x = 1;
	int32_t tile_step_y = 1;

	if (quadrant == 2 || quadrant == 3)
	{
		tile_step_x = -1;
		step_y.as_int *= -1;

		tile_x += tile_step_x;
		intercept_y = fixed64_add(intercept_y, step_y);
	}
	if (quadrant == 3 || quadrant == 4)
	{
		tile_step_y = -1;
		step_x.as_int *= -1;

		tile_y += tile_step_y;
		intercept_x = fixed64_add(intercept_x, step_x);
	}

	bool found_horiz_wall = false;
	bool found_vert_wall = false;

	double collision_coords[2] = {0, 0};
	while (!found_horiz_wall && !found_vert_wall) {
		if ((tile_step_x * intercept_x.as_int <= tile_step_x * ((int64_t) tile_x << 32)) && (intercept_x.as_int != (1L << 63))) {
			int32_t intercept_x_floor = intercept_x.as_int >> 32;

			int top_cell_material    = re_map_coords_in_bounds(map, intercept_x_floor, tile_y)
				? re_map_get_cell(map, intercept_x_floor, tile_y).material_bottom
				: out_of_bounds_material;

			int bottom_cell_material = re_map_coords_in_bounds(map, intercept_x_floor, tile_y - 1)
				? re_map_get_cell(map, intercept_x_floor, tile_y - 1).material_top
				: out_of_bounds_material;

			int material_close, material_far;
			if (quadrant == 1 || quadrant == 2) {
				material_close = bottom_cell_material;
				material_far = top_cell_material;
			} else {
				material_close = top_cell_material;
				material_far = bottom_cell_material;
			}

			if (material_close != transparent_material) {
				*collided_material = material_close;
				found_horiz_wall = true;
			} else if (material_far != transparent_material) {
				*collided_material = material_far;
				found_horiz_wall = true;
			}

			if (found_horiz_wall) {
				collision_coords[0] = fixed64_to_double(intercept_x);
				collision_coords[1] = tile_y;
			} else { // Step
				tile_y += tile_step_y;
				intercept_x = fixed64_add(intercept_x, step_x);
			}
		} else {
			int32_t intercept_y_floor = intercept_y.as_int >> 32;
			int right_cell_material = re_map_coords_in_bounds(map, tile_x, intercept_y_floor)
				? re_map_get_cell(map, tile_x, intercept_y_floor).material_left
				: out_of_bounds_material;
			int left_cell_material  = re_map_coords_in_bounds(map, tile_x - 1, intercept_y_floor)
				? re_map_get_cell(map, tile_x - 1, intercept_y_floor).material_right
				: out_of_bounds_material;

			int material_close, material_far;
			if (quadrant == 1 || quadrant == 4) {
				material_close = left_cell_material;
				material_far = right_cell_material;
			} else {
				material_close = right_cell_material;
				material_far = left_cell_material;
			}

			if (material_close != transparent_material) {
				*collided_material = material_close;
				found_vert_wall = true;
			} else if (material_far != transparent_material) {
				*collided_material = material_far;
				found_vert_wall = true;
			}

			if (found_vert_wall) {
				collision_coords[0] = tile_x;
				collision_coords[1] = fixed64_to_double(intercept_y);
			} else { // Step
				tile_x += tile_step_x;
				intercept_y = fixed64_add(intercept_y, step_y);
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

