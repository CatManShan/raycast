#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem-utils/mem-macros.h"
#include "raycast-engine/raycast-engine.h"

#ifdef MEM_DEBUG
#include "mem-utils/mem-debug.h"
#endif

int main()
{
	struct REMap *map = re_map_create(16, 16);
	re_map_destroy(map);

	return EXIT_SUCCESS;
}
