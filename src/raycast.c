#include <math.h>
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

static void map_init(struct REMap *map);
static void draw_frame(struct REMap *map, struct SCGBuffer *pixel_buffer, double origin_x, double origin_y, double forward_angle);
static void map_print(struct REMap *map);
static double coords_to_angle(double x, double y);
static int32_t min_int32(int32_t a, int32_t b);

enum Material {
	OUT_OF_BOUNDS = SCG_COLOR_BLACK,
	FLOOR = SCG_COLOR_DEFAULT,
	WALL = SCG_COLOR_BLUE
};

int main()
{
	struct REMap *map = re_map_create(16, 16);
	map_init(map);
	map_print(map);
	printf("\n");

	struct SCGBuffer *pixel_buffer = scg_pixel_buffer_create(64, 48);

	draw_frame(map, pixel_buffer, 8, 8, PI / 4);

	re_map_destroy(map);

	return EXIT_SUCCESS;
}

static void map_init(struct REMap *map)
{
	re_map_fill(map, FLOOR);

	for (size_t x = 4; x < 12; x++) {
		re_map_set_material(map, x, 11, WALL);
	}
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
		int32_t start = (screen_height - length) / 2;
		int32_t end = start + length;

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

static void map_print(struct REMap *map)
{
	for (uint32_t y = 0; y < map->height; y++) {
		for (uint32_t x = 0; x < map->width; x++) {
			printf("%d ", re_map_get_material(map, x, (map->height - 1) - y));
		}
		printf("\n");
	}
}

static double coords_to_angle(double x, double y)
{
	return atan2(y, x);
}

static int32_t min_int32(int32_t a, int32_t b)
{
	return (a < b) ? a : b;
}

