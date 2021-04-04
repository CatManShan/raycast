#ifndef maze_gen_h
#define maze_gen_h

#include <stdbool.h>
#include <stdint.h>

enum MazeCellWall {
	MAZE_WALL_TOP = 0,
	MAZE_WALL_RIGHT,
	MAZE_WALL_BOTTOM,
	MAZE_WALL_LEFT
};

struct Maze {
	uint32_t width;
	uint32_t height;
	struct MazeCell {
		bool visited : 1;
		uint8_t walls : 4;
	} cells[];
};

struct Maze *maze_create(uint32_t width, uint32_t height);
void maze_destroy(struct Maze *maze);

void maze_generate(struct Maze *maze);

struct MazeCell *maze_get_cell(struct Maze *maze, uint32_t col, uint32_t row);

bool maze_is_cell_visited(struct Maze *maze, uint32_t col, uint32_t row);
void maze_set_cell_visited(struct Maze *maze, uint32_t col, uint32_t row, bool visited);

bool maze_has_wall(struct Maze *maze, uint32_t col, uint32_t row, enum MazeCellWall wall);
void maze_add_wall(struct Maze *maze, uint32_t col, uint32_t row, enum MazeCellWall wall);
void maze_remove_wall(struct Maze *maze, uint32_t col, uint32_t row, enum MazeCellWall wall);

bool maze_is_position_in_bounds(struct Maze *maze, int64_t col, int64_t row);

#endif // maze_gen_h

