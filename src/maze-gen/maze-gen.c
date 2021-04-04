#include <stdbool.h>
#include <stdlib.h>

#include "../mem-utils/mem-macros.h"

#include "maze-gen.h"

struct MazeCellPosition {
	uint32_t col;
	uint32_t row;
	enum MazeCellWall shared_wall;
};

static void maze_init(struct Maze *maze, uint32_t width, uint32_t height);
static bool cell_has_wall(struct MazeCell *cell, enum MazeCellWall wall);
static void cell_add_wall(struct MazeCell *cell, enum MazeCellWall wall);
static void clear_cell_wall(struct MazeCell *cell, enum MazeCellWall wall);
static void get_cell_neighbor_position(int64_t col, int64_t row, enum MazeCellWall shared_wall, int64_t *neighbor_col, int64_t *neighbor_row);
static uint8_t get_cell_neighbor_positions(struct Maze *maze, uint32_t col, uint32_t row, struct MazeCellPosition *neighbor_positions);
static uint8_t get_cell_unvisited_neighbor_positions(struct Maze *maze, uint32_t col, uint32_t row,
		struct MazeCellPosition *unvisited_neighbor_positions);

struct Maze *maze_create(uint32_t width, uint32_t height)
{
	uint64_t size = (uint64_t) width * height;
	struct Maze *maze = ALLOC_FLEX_STRUCT(maze, cells, size);

	maze->width = width;
	maze->height = height;

	maze_init(maze, width, height);

	return maze;
}

void maze_destroy(struct Maze *maze)
{
	free(maze);
}

void maze_generate(struct Maze *maze)
{
	uint64_t position_stack_size = (uint64_t) maze->width * maze->height;
	uint64_t position_stack_length = 0;
	struct MazeCellPosition *position_stack = ALLOC_ARR(position_stack, position_stack_size);

	position_stack[0] = (struct MazeCellPosition) { maze->width / 2, maze->height / 2, -1 };
	position_stack_length++;
	maze_set_cell_visited(maze, position_stack[0].col, position_stack[0].row, true);

	while (position_stack_length > 0) {
		struct MazeCellPosition current_position = position_stack[position_stack_length - 1];

		struct MazeCellPosition unvisited_neighbor_positions[4];
		uint8_t unvisited_neighbor_count = get_cell_unvisited_neighbor_positions(maze, current_position.col, current_position.row,
				unvisited_neighbor_positions);

		if (unvisited_neighbor_count > 0) {
			uint8_t random_index = rand() % unvisited_neighbor_count;
			struct MazeCellPosition next_position = unvisited_neighbor_positions[random_index];

			maze_remove_wall(maze, current_position.col, current_position.row, next_position.shared_wall);

			position_stack[position_stack_length] = next_position;
			position_stack_length++;
			maze_set_cell_visited(maze, next_position.col, next_position.row, true);
		} else {
			position_stack_length--;
		}
	}

	maze_remove_wall(maze, 0, 0, MAZE_WALL_LEFT);
	maze_remove_wall(maze, maze->width - 1, maze->height - 1, MAZE_WALL_RIGHT);

	free(position_stack);
}

struct MazeCell *maze_get_cell(struct Maze *maze, uint32_t col, uint32_t row)
{
	uint64_t index = (uint64_t) row * maze->width + col;
	return &maze->cells[index];
}

bool maze_is_cell_visited(struct Maze *maze, uint32_t col, uint32_t row)
{
	return maze_get_cell(maze, col, row)->visited;
}

void maze_set_cell_visited(struct Maze *maze, uint32_t col, uint32_t row, bool visited)
{
	maze_get_cell(maze, col, row)->visited = visited;
}

bool maze_has_wall(struct Maze *maze, uint32_t col, uint32_t row, enum MazeCellWall wall)
{
	return cell_has_wall(maze_get_cell(maze, col, row), wall);
}

void maze_add_wall(struct Maze *maze, uint32_t col, uint32_t row, enum MazeCellWall wall)
{
	cell_add_wall(maze_get_cell(maze, col, row), wall);

	int64_t neighbor_col, neighbor_row;
	get_cell_neighbor_position(col, row, wall, &neighbor_col, &neighbor_row);

	if (maze_is_position_in_bounds(maze, neighbor_col, neighbor_row)) {
		enum MazeCellWall opposite_wall = (wall + 2) % 4;

		cell_add_wall(maze_get_cell(maze, neighbor_col, neighbor_row), opposite_wall);
	}
}

void maze_remove_wall(struct Maze *maze, uint32_t col, uint32_t row, enum MazeCellWall wall)
{
	clear_cell_wall(maze_get_cell(maze, col, row), wall);

	int64_t neighbor_col, neighbor_row;
	get_cell_neighbor_position(col, row, wall, &neighbor_col, &neighbor_row);

	if (maze_is_position_in_bounds(maze, neighbor_col, neighbor_row)) {
		enum MazeCellWall opposite_wall = (wall + 2) % 4;

		clear_cell_wall(maze_get_cell(maze, neighbor_col, neighbor_row), opposite_wall);
	}
}

bool maze_is_position_in_bounds(struct Maze *maze, int64_t col, int64_t row)
{
	return (col >= 0 && row >= 0 && col < maze->width && row < maze->height);
}

static void maze_init(struct Maze *maze, uint32_t width, uint32_t height)
{
	for (uint32_t index = 0; index < (width * height); index++) {
		maze->cells[index] = (struct MazeCell) {
			.visited = false,
			.walls = ~0
		};
	};
}

static bool cell_has_wall(struct MazeCell *cell, enum MazeCellWall wall)
{
	return (cell->walls & (1 << wall));
}

static void cell_add_wall(struct MazeCell *cell, enum MazeCellWall wall)
{
	cell->walls |= (1 << wall);
}

static void clear_cell_wall(struct MazeCell *cell, enum MazeCellWall wall)
{
	cell->walls &= ~(1 << wall);
}

static void get_cell_neighbor_position(int64_t col, int64_t row, enum MazeCellWall shared_wall, int64_t *neighbor_col, int64_t *neighbor_row)
{
	*neighbor_col = col + (shared_wall == MAZE_WALL_RIGHT) - (shared_wall == MAZE_WALL_LEFT);
	*neighbor_row = row + (shared_wall == MAZE_WALL_BOTTOM) - (shared_wall == MAZE_WALL_TOP);
}

static uint8_t get_cell_neighbor_positions(struct Maze *maze, uint32_t col, uint32_t row, struct MazeCellPosition *neighbor_positions)
{
		uint8_t neighbor_count = 0;
		if (col > 0) {
			neighbor_positions[neighbor_count] = (struct MazeCellPosition) { col - 1, row, MAZE_WALL_LEFT };
			++neighbor_count;
		}
		if (row > 0) {
			neighbor_positions[neighbor_count] = (struct MazeCellPosition) { col , row - 1, MAZE_WALL_TOP };
			++neighbor_count;
		}
		if (col < maze->width - 1) {
			neighbor_positions[neighbor_count] = (struct MazeCellPosition) { col + 1, row, MAZE_WALL_RIGHT };
			++neighbor_count;
		}
		if (row < maze->height - 1) {
			neighbor_positions[neighbor_count] = (struct MazeCellPosition) { col , row + 1, MAZE_WALL_BOTTOM };
			++neighbor_count;
		}

		return neighbor_count;
}

static uint8_t get_cell_unvisited_neighbor_positions(struct Maze *maze, uint32_t col, uint32_t row,
		struct MazeCellPosition *unvisited_neighbor_positions)
{
	struct MazeCellPosition neighbor_positions[4];
	uint8_t neighbor_count = get_cell_neighbor_positions(maze, col, row, neighbor_positions);

	uint8_t unvisited_neighbor_count = 0;
	for (uint8_t index = 0; index < neighbor_count; index++) {
		struct MazeCellPosition neighbor_position = neighbor_positions[index];
		if (!maze_is_cell_visited(maze, neighbor_position.col, neighbor_position.row)) {
			unvisited_neighbor_positions[unvisited_neighbor_count] = neighbor_position;
			unvisited_neighbor_count += 1;
		}
	}

	return unvisited_neighbor_count;
}

