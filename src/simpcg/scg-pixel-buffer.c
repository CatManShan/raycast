#include "simpcg.h" // Interface

struct SCGPixelBuffer *scg_pixel_buffer_create(uint8_t width, uint8_t height)
{
	struct SCGPixelBuffer *buffer = (struct SCGPixelBuffer *) scg_buffer_create(width * 2, height);

	scg_buffer_fill_ch(&buffer->internal_buffer, ' ');

	return buffer;
}

void scg_pixel_buffer_destroy(struct SCGPixelBuffer *buffer)
{
	scg_buffer_destroy(&buffer->internal_buffer);
}

void scg_pixel_buffer_set(struct SCGPixelBuffer *buffer, uint8_t col, uint8_t row, enum SCGColorCode color)
{
	scg_buffer_set_bg_color(&buffer->internal_buffer, col * 2, row, color);
	scg_buffer_set_bg_color(&buffer->internal_buffer, col * 2 + 1, row, color);
}

enum SCGColorCode scg_pixel_buffer_get(struct SCGPixelBuffer *buffer, uint8_t col, uint8_t row)
{
	return scg_buffer_get_bg_color(&buffer->internal_buffer, col * 2, row);
}

void scg_pixel_buffer_fill(struct SCGPixelBuffer *buffer, enum SCGColorCode color)
{
	scg_buffer_fill_bg_color(&buffer->internal_buffer, color);
}

void scg_pixel_buffer_make_space(struct SCGPixelBuffer *buffer)
{
	scg_buffer_make_space(&buffer->internal_buffer);
}

void scg_pixel_buffer_remove_space(struct SCGPixelBuffer *buffer)
{
	scg_buffer_remove_space(&buffer->internal_buffer);
}

void scg_pixel_buffer_print(struct SCGPixelBuffer *buffer)
{
	scg_buffer_print(&buffer->internal_buffer);
}

uint8_t scg_pixel_buffer_get_width(struct SCGPixelBuffer *buffer)
{
	return buffer->internal_buffer.width / 2;
}

uint8_t scg_pixel_buffer_get_height(struct SCGPixelBuffer *buffer)
{
	return buffer->internal_buffer.height / 2;
}

