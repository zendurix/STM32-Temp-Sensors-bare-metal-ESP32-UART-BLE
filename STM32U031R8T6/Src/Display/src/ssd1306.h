#ifndef DISPLAY_SRC_SSD1306_H
#define DISPLAY_SRC_SSD1306_H

#include <stdint.h>
#include <stdbool.h>

#define SSD1306_DISPLAY_WIDTH 128U
#define SSD1306_DISPLAY_HEIGHT 32U
#define SSD1306_PAGES_NUM 4U
#define SSD1306_PAGE_BYTE_SIZE 128U
#define SSD1306_BYTE_BUFFER_SIZE 512U

#define SSD1306_I2C_SLAVE_ADDR 0b0111100U
#define SSD1306_CONTROL_BYTE_COMMAND_MODE 0b00000000
#define SSD1306_CONTROL_BYTE_DATA_MODE 0b01000000

// ssd1306 is split into pages of 128x8 pixels.
typedef struct SSD1306DisplayPage {
	uint8_t page_num;
	uint8_t buffer[SSD1306_PAGE_BYTE_SIZE];

	bool is_dirty;
	bool is_empty;
} SSD1306DisplayPage;

typedef struct SSD1306Display {
	SSD1306DisplayPage pages[SSD1306_PAGES_NUM];

	bool is_ON;
} SSD1306Display;

// contrast 0-255
void SSD1306Display_initialize(SSD1306Display *display, uint8_t contrast);

void SSD1306Display_send_initialization_data(uint8_t contrast);

void SSD1306Display_clear_buffer(SSD1306Display *display);

void SSD1306Display_draw_buffer(SSD1306Display *display);

void SSD1306Display_draw_page(SSD1306DisplayPage *page);

void SSD1306Display_switch_page_writing(uint8_t page_num);

void SSD1306Display_set_pixel(SSD1306Display *display, uint8_t x,
		uint8_t y, bool pixel_on);

void SSD1306Display_toggle_on_off(SSD1306Display *display);

#endif /* DISPLAY_SRC_SSD1306_H */
