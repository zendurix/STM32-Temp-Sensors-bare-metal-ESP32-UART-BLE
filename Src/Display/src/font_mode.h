#ifndef DISPLAY_SRC_FONT_MODE_H
#define DISPLAY_SRC_FONT_MODE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct SSD1306Display SSD1306Display;

#define CHAR_PIXEL_SIZE 8
#define TEXT_COLS 16
#define TEXT_ROWS 4

typedef struct FontMode {
	char text[TEXT_ROWS][TEXT_COLS];

	bool is_dirty;
} FontMode;

void FontMode_init(FontMode *font_mode);

void FontMode_clear_text(FontMode *font_mode);

void FontMode_add_char_to_text(FontMode *font_mode, uint8_t character,
		uint8_t char_x, uint8_t char_y);

void FontMode_add_string_to_text(FontMode *font_mode, const char *msg,
		uint8_t start_x, uint8_t start_y);

void FontMode_draw_text(FontMode *font_mode, SSD1306Display *display);

void FontMode_add_char_to_buffer(SSD1306Display *display, uint8_t character,
		uint8_t char_x, uint8_t char_y);

#endif /* DISPLAY_SRC_FONT_MODE_H */
