#ifndef raycast_engine_h
#define raycast_engine_h

#include <stdint.h>

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

double re_raycast(uint32_t origin_x, uint32_t origin_y, double angle,
		uint32_t *collision_x, uint32_t collision_y);

#endif // raycast_engine_h

