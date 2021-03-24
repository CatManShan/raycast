#include "simpcg.h" // Interface

struct SCGBuffer *scg_pixel_buffer_create(uint8_t width, uint8_t height)
{
	struct SCGBuffer *buffer = scg_buffer_create(width * 2, height);

	scg_buffer_fill_ch(buffer, ' ');

	return buffer;
}

void scg_pixel_buffer_destroy(struct SCGBuffer *buffer)
{
	scg_buffer_destroy(buffer);
}

void scg_pixel_buffer_set(struct SCGBuffer *buffer, uint8_t col, uint8_t row, enum SCGColorCode color)
{
	scg_buffer_set_bg_color(buffer, col * 2, row, color);
	scg_buffer_set_bg_color(buffer, col * 2 + 1, row, color);
}

enum SCGColorCode scg_pixel_buffer_get(struct SCGBuffer *buffer, uint8_t col, uint8_t row)
{
	return scg_buffer_get_bg_color(buffer, col * 2, row);
}

void scg_pixel_buffer_fill(struct SCGBuffer *buffer, enum SCGColorCode color)
{
	scg_buffer_fill_bg_color(buffer, color);
}

void scg_pixel_buffer_make_space(struct SCGBuffer *buffer)
{
	scg_buffer_make_space(buffer);
}

void scg_pixel_buffer_remove_space(struct SCGBuffer *buffer)
{
	scg_buffer_remove_space(buffer);
}

void scg_pixel_buffer_print(struct SCGBuffer *buffer)
{
	scg_buffer_print(buffer);
}

uint8_t scg_pixel_buffer_get_width(struct SCGBuffer *buffer)
{
	return buffer->width / 2;
}

uint8_t scg_pixel_buffer_get_height(struct SCGBuffer *buffer)
{
	return buffer->height / 2;
}

