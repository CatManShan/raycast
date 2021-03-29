#ifndef raycast_engine_h
#define raycast_engine_h

#include <stdint.h>
#include <stdbool.h>

#define RE_MAP_CELL_SOLID(material) (struct REMapCell) { material, material, material, material }

struct REMap {
	uint32_t width;
	uint32_t height;

	struct REMapCell {
		int material_top;
		int material_right;
		int material_bottom;
		int material_left;
	} cells[];
};

struct REMap *re_map_create(uint32_t width, uint32_t height);
void re_map_destroy(struct REMap *map);

struct REMapCell re_map_get_cell(struct REMap *map, uint32_t x, uint32_t y);
void re_map_set_cell(struct REMap *map, uint32_t x, uint32_t y, struct REMapCell cell);
void re_map_fill(struct REMap *map, struct REMapCell cell);

double re_cast_ray(struct REMap *map, double origin_x, double origin_y, double forward_angle, double rel_angle,
		int transparent_material, int out_of_bounds_material, int *collided_material);

bool re_map_coords_in_bounds(struct REMap *map, int64_t x, int64_t y);

#endif // raycast_engine_h

