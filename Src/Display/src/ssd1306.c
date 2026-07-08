#include "ssd1306.h"

#include "i2c/i2c.h"

void SSD1306Display_initialize(SSD1306Display *display, uint8_t contrast) {
	if (!display) {
		return;
	}
	display->is_ON = true;

	for (uint8_t page_num = 0; page_num < SSD1306_PAGES_NUM; page_num++) {
		display->pages[page_num].page_num = page_num;
		display->pages[page_num].is_empty = false;
		display->pages[page_num].is_dirty = true;
	}

	I2C_configure();
	SSD1306Display_send_initialization_data(contrast);
	SSD1306Display_clear_buffer(display);
}

void SSD1306Display_send_initialization_data(uint8_t contrast) {
#define INIT_DATA_SIZE 24
	I2C_init_transfer(INIT_DATA_SIZE, SSD1306_I2C_SLAVE_ADDR);
	uint8_t initialization_data_transfer[INIT_DATA_SIZE] = { // newline
			// newline
					SSD1306_CONTROL_BYTE_COMMAND_MODE,// CONTROL byte

					0xA8, 31, // set Multiplexer ratio (31 for 128x32 display)

					0xD3, 0,  // set display offset, 0 - no offset

					0x40, // set display start line - 0

					0xA1, // set segment re-map

					0xC8, // set COM output scan direction

					0xDA, 0x02, // set COM pins hardware configuration

					0x81, contrast, // set contrast control (0-255)

					0xA4, // set entire display ON - A5, Off (display from RAM - A4)

					0xA6, // set normal display

					0xD5, 0b10000000, // set display clock divide ratio / oscillator frequency, (RESET value)

					0x20, 0x02, // set memory addressing mode 2 - page addressing mode

					0x8D, 0x14, // Enable charge pump regulator

					0xAF, // DISPLAY ON !!!
			};

	for (uint8_t i = 0; i < INIT_DATA_SIZE; i++) {
		I2C_send_byte(initialization_data_transfer[i]);
	}

	I2C_wait_for_transfer_end();
}

void SSD1306Display_clear_buffer(SSD1306Display *display) {
	if (!display) {
		return;
	}

	for (uint8_t page_num = 0; page_num < SSD1306_PAGES_NUM; page_num++) {
		if (display->pages[page_num].is_empty) {
			continue;
		}

		for (uint16_t i = 0; i < SSD1306_PAGE_BYTE_SIZE; i++) {
			display->pages[page_num].buffer[i] = 0;
		}
		display->pages[page_num].is_empty = true;
		display->pages[page_num].is_dirty = true;
	}
}

void SSD1306Display_draw_buffer(SSD1306Display *display) {
	if (!display) {
		return;
	}

	for (uint8_t page_num = 0; page_num < SSD1306_PAGES_NUM; page_num++) {
		if (!display->pages[page_num].is_dirty) {
			continue;
		}

		SSD1306Display_switch_page_writing(page_num);
		SSD1306Display_draw_page(&display->pages[page_num]);
		display->pages[page_num].is_dirty = false;
	}
}

void SSD1306Display_draw_page(SSD1306DisplayPage *page) {
	if (!page) {
		return;
	}

	I2C_init_transfer(SSD1306_PAGE_BYTE_SIZE + 1, SSD1306_I2C_SLAVE_ADDR);
	I2C_send_byte(SSD1306_CONTROL_BYTE_DATA_MODE);

	for (uint16_t i = 0; i < SSD1306_PAGE_BYTE_SIZE; i++) {
		I2C_send_byte(page->buffer[i]);
	}
	I2C_wait_for_transfer_end();
}

void SSD1306Display_switch_page_writing(uint8_t page_num) {
#define SWITCH_DATA_SIZE 4
	I2C_init_transfer(SWITCH_DATA_SIZE, SSD1306_I2C_SLAVE_ADDR);
	uint8_t switch_page_data_transfer[SWITCH_DATA_SIZE] = { // newline
			// newline
					SSD1306_CONTROL_BYTE_COMMAND_MODE,//
					0xB0 + page_num, // COMMAND Setup page start
					0x00, // COMMAND column address lower part  [3:0]
					0x10, // COMMAND column address higher part [7:4]
			};

	for (uint8_t i = 0; i < SWITCH_DATA_SIZE; i++) {
		I2C_send_byte(switch_page_data_transfer[i]);
	}
	I2C_wait_for_transfer_end();
}

void SSD1306Display_set_pixel(SSD1306Display *display, uint8_t x, uint8_t y,
bool pixel_on) {
	if (!display || x >= SSD1306_DISPLAY_WIDTH || y >= SSD1306_DISPLAY_HEIGHT) {
		return;
	}

	const uint8_t page_num = y / 8;
	const uint8_t byte_bit = y % 8;

	uint8_t *byte = &display->pages[page_num].buffer[x];

	if (pixel_on) {
		display->pages[page_num].is_empty = false;
		*byte |= (1 << byte_bit);
	} else {
		*byte &= ~(1 << byte_bit);
	}

	display->pages[page_num].is_dirty = true;
}

void SSD1306Display_toggle_on_off(SSD1306Display *display) {
	if (!display) {
		return;
	}

	display->is_ON = !display->is_ON;

	I2C_init_transfer(2, SSD1306_I2C_SLAVE_ADDR);

	I2C_send_byte(SSD1306_CONTROL_BYTE_COMMAND_MODE);

	if (display->is_ON) {
		I2C_send_byte(0xAF);
	} else {
		I2C_send_byte(0xAE);
	}

	I2C_wait_for_transfer_end();
}
