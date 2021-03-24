#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../mem-utils/mem-macros.h"

#ifdef MEM_DEBUG
#include "../mem-utils/mem-debug.h"
#endif // MEM_DEBUG

#include "raycast-engine.h"

#define THRESHOLD 0x0.001p0
#define PI 3.14159265358979323846

static double reduce_angle(double angle);
static double distance_of_points(double x1, double y1, double x2, double y2);
static bool double_less_than_or_equal(double a, double b);
static bool double_equal_within_threshold(double a, double b, double threshold);

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

int re_map_get_material(struct REMap *map, uint32_t x, uint32_t y)
{
	return map->cells[y * map->width + x].material;
}

void re_map_set_material(struct REMap *map, uint32_t x, uint32_t y, int material)
{
	map->cells[y * map->width + x].material = material;
}

void re_map_fill(struct REMap *map, int material)
{
	uint64_t area = (uint64_t) map->width * map->height;
	for (uint64_t index = 0; index < area; index++) {
		map->cells[index].material = material;
	}
}

double re_raycast(struct REMap *map, double origin_x, double origin_y, double forward_angle, double rel_angle,
		int transparent_material, int out_of_bounds_material, int *collided_material)
{
	int32_t origin_x_whole = (int32_t) origin_x;
	int32_t origin_y_whole = (int32_t) origin_y;
	double origin_x_frac = origin_x - origin_x_whole;
	double origin_y_frac = origin_y - origin_y_whole;
	double absolute_angle = reduce_angle(forward_angle + rel_angle);

#if UNUSED == 1
	bool quad1 = absolute_angle >= 0 && absolute_angle < PI / 2;
#endif
	bool quad2 = absolute_angle >= PI / 2 && absolute_angle < PI;
	bool quad3 = absolute_angle >= PI && absolute_angle < PI * 3 / 2;
	bool quad4 = absolute_angle >= PI * 3 / 2 && absolute_angle < PI * 2;

	double x_intercept = origin_x_whole + origin_x_frac + (1 - origin_y_frac) / tan(absolute_angle);
	double y_intercept = origin_y_whole + origin_y_frac + (1 - origin_x_frac) * tan(absolute_angle);

	int32_t check_x = origin_x_whole + 1;
	int32_t check_y = origin_y_whole + 1;

	double step_x = 1 / tan(absolute_angle);
	double step_y = tan(absolute_angle);

	int32_t tile_step_x = 1;
	int32_t tile_step_y = 1;

	if (quad2 || quad3)
	{
		tile_step_x *= -1;
		step_y *= -1;

		check_x += tile_step_x;
		y_intercept += step_y;
	}
	if (quad3 || quad4)
	{
		tile_step_y *= -1;
		step_x *= -1;

		check_y += tile_step_y;
		x_intercept += step_x;
	}

	bool found_horiz_wall = false;
	bool found_vert_wall = false;

	double found_coords[2] = {0, 0};
	double texture_unit_phase = 0;
	bool out_of_bounds = false;

	*collided_material = out_of_bounds_material;

	while (!found_horiz_wall && !found_vert_wall && !out_of_bounds) {
		while (double_less_than_or_equal(tile_step_x * x_intercept, tile_step_x * check_x) && !found_horiz_wall /* && !found_vert_wall  && !out_of_bounds*/) {
			if (!re_map_coords_in_bounds(map, (int32_t) x_intercept, check_y)) { // Check if bottom or top is out-of-bounds
				found_horiz_wall = true;
				out_of_bounds = true;

				found_coords[0] = x_intercept;
				found_coords[1] = check_y;

				texture_unit_phase = x_intercept;

				*collided_material = out_of_bounds_material;
			} else if (!re_map_coords_in_bounds(map, (int32_t) x_intercept, check_y - 1)) { // Check if bottom or top is out-of-bounds
				found_horiz_wall = true;
				out_of_bounds = true;

				found_coords[0] = x_intercept;
				found_coords[1] = check_y;

				*collided_material = out_of_bounds_material;
			} else if (re_map_get_material(map, (int32_t) x_intercept, check_y) != transparent_material) { // Check if bottom is wall
				found_horiz_wall = true;
				found_coords[0] = x_intercept;
				found_coords[1] = check_y;

				texture_unit_phase = x_intercept;

				*collided_material = re_map_get_material(map, (int32_t) x_intercept, check_y);
			} else if (check_y > 0 && re_map_get_material(map, (int32_t) x_intercept, check_y - 1) != transparent_material) { // Check if top is wall
				found_horiz_wall = true;
				found_coords[0] = x_intercept;
				found_coords[1] = check_y;

				*collided_material = re_map_get_material(map, (int32_t) x_intercept, check_y - 1);
			} else { // Step
				check_y += tile_step_y;
				x_intercept += step_x;
			}
		}

		while (double_less_than_or_equal(tile_step_y * y_intercept, tile_step_y * check_y) && !found_horiz_wall && !found_vert_wall /*&& !out_of_bounds*/)
		{
			if (!re_map_coords_in_bounds(map, check_x, (int32_t) y_intercept)) { // Check if left is out-of-bounds
				found_vert_wall = true;
				out_of_bounds = true;

				found_coords[0] = check_x;
				found_coords[1] = y_intercept;

				*collided_material = out_of_bounds_material;
			} else if (!re_map_coords_in_bounds(map, check_x - 1, (int32_t) y_intercept)) { // Check if right is out-of-bounds
				found_vert_wall = true;
				out_of_bounds = true;

				found_coords[0] = check_x;
				found_coords[1] = y_intercept;

				texture_unit_phase = y_intercept;

				*collided_material = out_of_bounds_material;
			} else if (re_map_get_material(map, check_x, (int32_t) y_intercept) != transparent_material) { // Check if left is wall
				found_vert_wall = true;
				found_coords[0] = check_x;
				found_coords[1] = y_intercept;

				*collided_material = re_map_get_material(map, check_x, (int32_t) y_intercept);
			} else if (check_x > 0 && re_map_get_material(map, check_x - 1, (int32_t) y_intercept) != transparent_material) { // Check if right is wall
				found_vert_wall = true;
				found_coords[0] = check_x;
				found_coords[1] = y_intercept;

				texture_unit_phase = y_intercept;

				*collided_material = re_map_get_material(map, check_x - 1, (int32_t) y_intercept);
			} else { // Step
				check_x += tile_step_x;
				y_intercept += step_y;
			}
		}
	}

	double check_distance = distance_of_points(origin_x, origin_y, found_coords[0], found_coords[1]);
	double forward_distance = check_distance * cos(rel_angle);

	texture_unit_phase = fmax(0, texture_unit_phase);

#if UNUSED == 1
	enum REOrientation wall_orientation = (found_horiz_wall ? RE_HORIZONTAL : RE_VERTICAL);
#endif

	// return new Ray(forward_distance, collided_material, texture_unit_phase, intercept_number, wall_orientation);

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
	return a <= b || double_equal_within_threshold(a, b, THRESHOLD);
}

static bool double_equal_within_threshold(double a, double b, double threshold)
{
	return fabs(a - b) < threshold;
}


