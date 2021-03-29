#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "../mem-utils/mem-macros.h"

#ifdef MEM_DEBUG
#include "../mem-utils/mem-debug.h"
#endif

#include "simptg.h"

static void stg_print_cell(struct SCGCell cell);
static uint16_t stg_color_code_to_ansi_fg(enum SCGColorCode color_code);
static uint16_t stg_color_code_to_ansi_bg(enum SCGColorCode color_code);

struct SCGBuffer *stg_buffer_create(uint16_t width, uint16_t height)
{
	size_t cells_size = width * height;
	struct SCGBuffer *buffer = ALLOC_FLEX_STRUCT(buffer, cells, cells_size);

	buffer->width = width;
	buffer->height = height;

	return buffer;
}

void stg_buffer_destroy(struct SCGBuffer *buffer)
{
	free(buffer);
}

inline void stg_buffer_set_ch(struct SCGBuffer *buffer, uint16_t col, uint16_t row, char ch)
{
	buffer->cells[row * buffer->width + col].ch = ch;
}

inline char stg_buffer_get_ch(struct SCGBuffer *buffer, uint16_t col, uint16_t row)
{
	return buffer->cells[row * buffer->width + col].ch;
}

inline void stg_buffer_set_fg_color(struct SCGBuffer *buffer, uint16_t col, uint16_t row, enum SCGColorCode fg_color)
{
	buffer->cells[row * buffer->width + col].fg_color = fg_color;
}

inline enum SCGColorCode stg_buffer_get_fg_color(struct SCGBuffer *buffer, uint16_t col, uint16_t row)
{
	return buffer->cells[row * buffer->width + col].fg_color;
}

inline void stg_buffer_set_bg_color(struct SCGBuffer *buffer, uint16_t col, uint16_t row, enum SCGColorCode bg_color)
{
	buffer->cells[row * buffer->width + col].bg_color = bg_color;
}

inline enum SCGColorCode stg_buffer_get_bg_color(struct SCGBuffer *buffer, uint16_t col, uint16_t row)
{
	return buffer->cells[row * buffer->width + col].bg_color;
}

void stg_buffer_fill_ch(struct SCGBuffer *buffer, char ch)
{
	uint16_t width = buffer->width;
	uint16_t height = buffer->height;
	for (uint16_t row = 0; row < height; row++) {
		for (uint16_t col = 0; col < width; col++) {
			stg_buffer_set_ch(buffer, col, row, ch);
		}
	}
}

void stg_buffer_fill_fg_color(struct SCGBuffer *buffer, enum SCGColorCode fg_color)
{
	uint16_t width = buffer->width;
	uint16_t height = buffer->height;
	for (uint16_t row = 0; row < height; row++) {
		for (uint16_t col = 0; col < width; col++) {
			stg_buffer_set_fg_color(buffer, col, row, fg_color);
		}
	}
}

void stg_buffer_fill_bg_color(struct SCGBuffer *buffer, enum SCGColorCode bg_color)
{
	uint16_t width = buffer->width;
	uint16_t height = buffer->height;
	for (uint16_t row = 0; row < height; row++) {
		for (uint16_t col = 0; col < width; col++) {
			stg_buffer_set_bg_color(buffer, col, row, bg_color);
		}
	}
}

void stg_buffer_make_space(struct SCGBuffer *buffer)
{
	for (uint16_t row = 0; row < buffer->height; row++) {
		printf("\n\x1b[G"); // Add <height> lines to bottom of console
	}
}

void stg_buffer_remove_space(struct SCGBuffer *buffer)
{
	printf("\x1b[%dA", buffer->height); // Move to top of buffer
	printf("\x1b[G"); // Move to 1st column
	printf("\x1b[J"); // Clear to bottom line
}

void stg_buffer_print(struct SCGBuffer *buffer)
{
	uint16_t width = buffer->width;
	uint16_t height = buffer->height;

	printf("\x1b[G"); // Move to 1st column
	printf("\x1b[%dA", height); // Move to top of buffer
	for (uint16_t row = 0; row < height; row++) {
		for (uint16_t col = 0; col < width; col++) {
			stg_print_cell(buffer->cells[row * width + col]);
		}
		printf("\x1b[B"); // Move down 1 line
		printf("\x1b[G"); // Move to 1st column
	}

	fflush(stdout);
}

// Static Functions

static void stg_print_cell(struct SCGCell cell)
{
	uint16_t ansi_fg_code = stg_color_code_to_ansi_fg(cell.fg_color);
	uint16_t ansi_bg_code = stg_color_code_to_ansi_bg(cell.bg_color);
	printf("\x1b[%d;%dm%c\x1b[0m", ansi_fg_code, ansi_bg_code, cell.ch); // Print char with specified fg and bg color
}

static uint16_t stg_color_code_to_ansi_fg(enum SCGColorCode color_code)
{
	return color_code + 39;
}

static uint16_t stg_color_code_to_ansi_bg(enum SCGColorCode color_code)
{
	return color_code + 49;
}

int stg_input_adjust()
{
	return system("stty raw -echo");
}

int stg_input_restore()
{
	return system("stty cooked echo");
}

