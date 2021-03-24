#ifndef raycast_engine_h
#define raycast_engine_h

#include <stdint.h>
#include <stdbool.h>

struct REMap {
	uint32_t width;
	uint32_t height;

	struct REMapCell {
		int material; // using int to be compatible with enums
	} cells[];
};

struct REMap *re_map_create(uint32_t width, uint32_t height);
void re_map_destroy(struct REMap *map);

int re_map_get_material(struct REMap *map, uint32_t x, uint32_t y);
void re_map_set_material(struct REMap *map, uint32_t x, uint32_t y, int material);
void re_map_fill(struct REMap *map, int material);

double re_raycast(struct REMap *map, double origin_x, double origin_y, double forward_angle, double rel_angle,
		int transparent_material, int out_of_bounds_material, int *collided_material);

bool re_map_coords_in_bounds(struct REMap *map, int64_t x, int64_t y);

#endif // raycast_engine_h

