#include "simptg.h"

struct SCGBuffer *stg_pixel_buffer_create(uint16_t width, uint16_t height)
{
	struct SCGBuffer *buffer = stg_buffer_create(width * 2, height);

	stg_buffer_fill_ch(buffer, ' ');

	return buffer;
}

void stg_pixel_buffer_destroy(struct SCGBuffer *buffer)
{
	stg_buffer_destroy(buffer);
}

void stg_pixel_buffer_set(struct SCGBuffer *buffer, uint16_t col, uint16_t row, enum SCGColorCode color)
{
	stg_buffer_set_bg_color(buffer, col * 2, row, color);
	stg_buffer_set_bg_color(buffer, col * 2 + 1, row, color);
}

enum SCGColorCode stg_pixel_buffer_get(struct SCGBuffer *buffer, uint16_t col, uint16_t row)
{
	return stg_buffer_get_bg_color(buffer, col * 2, row);
}

void stg_pixel_buffer_fill(struct SCGBuffer *buffer, enum SCGColorCode color)
{
	stg_buffer_fill_bg_color(buffer, color);
}

void stg_pixel_buffer_make_space(struct SCGBuffer *buffer)
{
	stg_buffer_make_space(buffer);
}

void stg_pixel_buffer_remove_space(struct SCGBuffer *buffer)
{
	stg_buffer_remove_space(buffer);
}

void stg_pixel_buffer_print(struct SCGBuffer *buffer)
{
	stg_buffer_print(buffer);
}

uint16_t stg_pixel_buffer_get_width(struct SCGBuffer *buffer)
{
	return buffer->width / 2;
}

uint16_t stg_pixel_buffer_get_height(struct SCGBuffer *buffer)
{
	return buffer->height / 2;
}

