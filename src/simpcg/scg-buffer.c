#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "../mem-utils/mem-macros.h"

#ifdef MEM_DEBUG
#include "../mem-utils/mem-debug.h"
#endif

#include "simpcg.h"

static void scg_print_cell(struct SCGCell cell);
static uint8_t scg_color_code_to_ansi_fg(enum SCGColorCode color_code);
static uint8_t scg_color_code_to_ansi_bg(enum SCGColorCode color_code);

struct SCGBuffer *scg_buffer_create(uint8_t width, uint8_t height)
{
	size_t cells_size = width * height;
	struct SCGBuffer *buffer = ALLOC_FLEX_STRUCT(buffer, cells, cells_size);

	buffer->width = width;
	buffer->height = height;

	return buffer;
}

void scg_buffer_destroy(struct SCGBuffer *buffer)
{
	free(buffer);
}

inline void scg_buffer_set_ch(struct SCGBuffer *buffer, uint8_t col, uint8_t row, char ch)
{
	buffer->cells[row * buffer->width + col].ch = ch;
}

inline char scg_buffer_get_ch(struct SCGBuffer *buffer, uint8_t col, uint8_t row)
{
	return buffer->cells[row * buffer->width + col].ch;
}

inline void scg_buffer_set_fg_color(struct SCGBuffer *buffer, uint8_t col, uint8_t row, enum SCGColorCode fg_color)
{
	buffer->cells[row * buffer->width + col].fg_color = fg_color;
}

inline enum SCGColorCode scg_buffer_get_fg_color(struct SCGBuffer *buffer, uint8_t col, uint8_t row)
{
	return buffer->cells[row * buffer->width + col].fg_color;
}

inline void scg_buffer_set_bg_color(struct SCGBuffer *buffer, uint8_t col, uint8_t row, enum SCGColorCode bg_color)
{
	buffer->cells[row * buffer->width + col].bg_color = bg_color;
}

inline enum SCGColorCode scg_buffer_get_bg_color(struct SCGBuffer *buffer, uint8_t col, uint8_t row)
{
	return buffer->cells[row * buffer->width + col].bg_color;
}

void scg_buffer_fill_ch(struct SCGBuffer *buffer, char ch)
{
	uint8_t width = buffer->width;
	uint8_t height = buffer->height;
	for (uint8_t row = 0; row < height; row++) {
		for (uint8_t col = 0; col < width; col++) {
			scg_buffer_set_ch(buffer, col, row, ch);
		}
	}
}

void scg_buffer_fill_fg_color(struct SCGBuffer *buffer, enum SCGColorCode fg_color)
{
	uint8_t width = buffer->width;
	uint8_t height = buffer->height;
	for (uint8_t row = 0; row < height; row++) {
		for (uint8_t col = 0; col < width; col++) {
			scg_buffer_set_fg_color(buffer, col, row, fg_color);
		}
	}
}

void scg_buffer_fill_bg_color(struct SCGBuffer *buffer, enum SCGColorCode bg_color)
{
	uint8_t width = buffer->width;
	uint8_t height = buffer->height;
	for (uint8_t row = 0; row < height; row++) {
		for (uint8_t col = 0; col < width; col++) {
			scg_buffer_set_bg_color(buffer, col, row, bg_color);
		}
	}
}

void scg_buffer_make_space(struct SCGBuffer *buffer)
{
	for (uint8_t row = 0; row < buffer->height; row++) {
		printf("\n\x1b[G"); // Add <height> lines to bottom of console
	}
}

void scg_buffer_remove_space(struct SCGBuffer *buffer)
{
	printf("\x1b[%dA", buffer->height); // Move to top of buffer
	printf("\x1b[G"); // Move to 1st column
	printf("\x1b[J"); // Clear to bottom line
}

void scg_buffer_print(struct SCGBuffer *buffer)
{
	uint8_t width = buffer->width;
	uint8_t height = buffer->height;

	printf("\x1b[G"); // Move to 1st column
	printf("\x1b[%dA", height); // Move to top of buffer
	for (uint8_t row = 0; row < height; row++) {
		for (uint8_t col = 0; col < width; col++) {
			scg_print_cell(buffer->cells[row * width + col]);
		}
		printf("\x1b[B"); // Move down 1 line
		printf("\x1b[G"); // Move to 1st column
	}

	fflush(stdout);
}

// Static Functions

static void scg_print_cell(struct SCGCell cell)
{
	uint8_t ansi_fg_code = scg_color_code_to_ansi_fg(cell.fg_color);
	uint8_t ansi_bg_code = scg_color_code_to_ansi_bg(cell.bg_color);
	printf("\x1b[%d;%dm%c\x1b[0m", ansi_fg_code, ansi_bg_code, cell.ch); // Print char with specified fg and bg color
}

static uint8_t scg_color_code_to_ansi_fg(enum SCGColorCode color_code)
{
	return color_code + 39;
}

static uint8_t scg_color_code_to_ansi_bg(enum SCGColorCode color_code)
{
	return color_code + 49;
}

void scg_input_adjust()
{
	system("stty raw -echo");
}

void scg_input_restore()
{
	system("stty cooked echo");
}

