#ifndef simpcg_h
#define simpcg_h

#include <stdint.h>

enum SCGColorCode {
	SCG_COLOR_DEFAULT = 0,

	SCG_COLOR_BLACK   = -9,
	SCG_COLOR_RED     = -8,
	SCG_COLOR_GREEN   = -7,
	SCG_COLOR_YELLOW  = -6,
	SCG_COLOR_BLUE    = -5,
	SCG_COLOR_MAGENTA = -4,
	SCG_COLOR_CYAN    = -3,
	SCG_COLOR_WHITE   = -2,

	SCG_COLOR_BRIGHT_BLACK   = 51,
	SCG_COLOR_BRIGHT_RED     = 52,
	SCG_COLOR_BRIGHT_GREEN   = 53,
	SCG_COLOR_BRIGHT_YELLOW  = 54,
	SCG_COLOR_BRIGHT_BLUE    = 55,
	SCG_COLOR_BRIGHT_MAGENTA = 56,
	SCG_COLOR_BRIGHT_CYAN    = 57,
	SCG_COLOR_BRIGHT_WHITE   = 58
};

/*** SCGBuffer ***/

struct SCGBuffer {
	uint8_t width;
	uint8_t height;
	struct SCGCell {
		char ch;
		enum SCGColorCode fg_color : 8;
		enum SCGColorCode bg_color : 8;
	} cells[];
};

struct SCGBuffer *scg_buffer_create(uint8_t width, uint8_t height);
void scg_buffer_destroy(struct SCGBuffer *buffer);

void scg_buffer_set_ch(struct SCGBuffer *buffer, uint8_t col, uint8_t row, char ch);
char scg_buffer_get_ch(struct SCGBuffer *buffer, uint8_t col, uint8_t row);
void scg_buffer_set_fg_color(struct SCGBuffer *buffer, uint8_t col, uint8_t row, enum SCGColorCode fg_color);
enum SCGColorCode scg_buffer_get_fg_color(struct SCGBuffer *buffer, uint8_t col, uint8_t row);
void scg_buffer_set_bg_color(struct SCGBuffer *buffer, uint8_t col, uint8_t row, enum SCGColorCode bg_color);
enum SCGColorCode scg_buffer_get_bg_color(struct SCGBuffer *buffer, uint8_t col, uint8_t row);

void scg_buffer_fill_ch(struct SCGBuffer *buffer, char ch);
void scg_buffer_fill_fg_color(struct SCGBuffer *buffer, enum SCGColorCode fg_color);
void scg_buffer_fill_bg_color(struct SCGBuffer *buffer, enum SCGColorCode bg_color);

void scg_buffer_make_space(struct SCGBuffer *buffer);
void scg_buffer_remove_space(struct SCGBuffer *buffer);
void scg_buffer_print(struct SCGBuffer *buffer);

int scg_input_adjust();
int scg_input_restore();

/*** SCGPixelBuffer ***/

struct SCGBuffer *scg_pixel_buffer_create(uint8_t width, uint8_t height); void scg_pixel_buffer_destroy(struct SCGBuffer *pixel_buffer);

void scg_pixel_buffer_set(struct SCGBuffer *pixel_buffer, uint8_t col, uint8_t row, enum SCGColorCode color);
enum SCGColorCode scg_pixel_buffer_get(struct SCGBuffer *pixel_buffer, uint8_t col, uint8_t row);

void scg_pixel_buffer_fill(struct SCGBuffer *pixel_buffer, enum SCGColorCode color);

void scg_pixel_buffer_make_space(struct SCGBuffer *pixel_buffer);
void scg_pixel_buffer_remove_space(struct SCGBuffer *pixel_buffer);

void scg_pixel_buffer_print(struct SCGBuffer *pixel_buffer);

#endif // simpcg_h

