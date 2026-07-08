#include "font_mode.h"

#include "ssd1306.h"
#include "resources/font_8x8_bitmap.h"

void FontMode_init(FontMode *font_mode) {
	if (!font_mode) {
		return;
	}
	font_mode->is_dirty = true;

	FontMode_clear_text(font_mode);
}

void FontMode_clear_text(FontMode *font_mode) {
	if (!font_mode) {
		return;
	}

	for (int row = 0; row < TEXT_ROWS; row++) {
		for (int col = 0; col < TEXT_COLS; col++) {
			font_mode->text[row][col] = ' ';
		}
	}
	font_mode->is_dirty = true;
}

void FontMode_add_char_to_text(FontMode *font_mode, uint8_t character,
		uint8_t char_x, uint8_t char_y) {
	if (!font_mode || char_x >= TEXT_COLS || char_y >= TEXT_ROWS
			|| character >= 128) {
		return;
	}

	font_mode->text[char_y][char_x] = character;
	font_mode->is_dirty = true;
}

void FontMode_add_string_to_text(FontMode *font_mode, const char *msg,
		uint8_t start_x, uint8_t start_y) {

	if (!font_mode || !msg || start_x >= TEXT_COLS || start_y >= TEXT_ROWS) {
		return;
	}

	uint8_t x = start_x;
	uint8_t y = start_y;

	for (const char *character = msg; *character != '\0'; character++) {
		if (*character < 128) {
			font_mode->text[y][x] = *character;
		} else {
			font_mode->text[y][x] = '?';
		}

		x++;
		if (x >= TEXT_COLS) {
			y++;
			if (y >= TEXT_ROWS) {
				break;
			}
			x = 0;
		}
	}
	font_mode->is_dirty = true;
}

void FontMode_draw_text(FontMode *font_mode, SSD1306Display *display) {
	if (!display || !font_mode || !font_mode->is_dirty) {
		return;
	}

	SSD1306Display_clear_buffer(display);
	for (int row = 0; row < TEXT_ROWS; row++) {
		for (int col = 0; col < TEXT_COLS; col++) {
			FontMode_add_char_to_buffer(display, font_mode->text[row][col], col,
					row);
		}
	}

	SSD1306Display_draw_buffer(display);
	font_mode->is_dirty = false;
}

void FontMode_add_char_to_buffer(SSD1306Display *display, uint8_t character,
		uint8_t char_x, uint8_t char_y) {
	if (!display || char_x >= TEXT_COLS || char_y >= TEXT_ROWS
			|| character >= 128) {
		return;
	}

	// offset (not used)
	//int16_t x_offset = (char_x + 1) * 1 + CHAR_PIXEL_SIZE * char_x;
	//int16_t y_offset = (char_y + 1) * 2 + CHAR_PIXEL_SIZE * char_y;

	int16_t x_offset = CHAR_PIXEL_SIZE * char_x;
	int16_t y_offset = CHAR_PIXEL_SIZE * char_y;

	for (uint8_t y = 0; y < 8; y++) {
		for (uint8_t x = 0; x < 8; x++) {

			if (font8x8_basic[character][y] & (1 << x)) {
				SSD1306Display_set_pixel(display, x + x_offset, y + y_offset,
				true);
			}
		}
	}

}

