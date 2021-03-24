#include <stdlib.h>

#include "../mem-utils/mem-macros.h"

#include "raycast-engine.h"

struct REMap *re_map_create(uint32_t width, uint32_t height)
{
	size_t area = (size_t) width * height;
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

