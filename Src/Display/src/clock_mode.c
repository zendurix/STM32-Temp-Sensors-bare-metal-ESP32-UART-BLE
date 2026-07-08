#include "clock_mode.h"

#include "ssd1306.h"
#include "resources/clock_digits.h"

void ClockHHMM_init(ClockHHMM *clock, uint8_t hour, uint8_t minute,
		uint32_t milliseconds) {
	if (!clock) {
		return;
	}

	clock->hour = hour;
	clock->minute = minute;

	clock->last_minute_time_stamp = milliseconds;
	clock->is_dirty = true;
}

void ClockHHMM_set_hh_mm(ClockHHMM *clock, uint8_t hour, uint8_t minute) {
	if (!clock) {
		return;
	}

	clock->hour = hour;
	clock->minute = minute;
	clock->is_dirty = true;
}

void ClockHHMM_update(ClockHHMM *clock, uint32_t milliseconds) {
	if (!clock) {
		return;
	}

	bool minute_passed = (milliseconds - clock->last_minute_time_stamp)
			>= 60000;

	if (minute_passed) {
		clock->is_dirty = true;
		clock->minute++;
		clock->last_minute_time_stamp = milliseconds;

		if (clock->minute >= 60) {
			clock->minute = 0;
			clock->hour++;

			if (clock->hour >= 24) {
				clock->hour = 0;
			}
		}
	}
}

void ClockHHMM_update_display(ClockHHMM *clock, SSD1306Display *display) {
	if (!clock || !display || !clock->is_dirty) {
		return;
	}

	SSD1306Display_clear_buffer(display);

	int8_t h0 = clock->hour / 10;
	int8_t h1 = clock->hour % 10;

	int8_t m0 = clock->minute / 10;
	int8_t m1 = clock->minute % 10;

	ClockHHMM_add_to_buffer_digit_pixels(display, digits_pixels[h0],
			digits_pixels_num[h0], DigitPos_HH_0);
	ClockHHMM_add_to_buffer_digit_pixels(display, digits_pixels[h1],
			digits_pixels_num[h1], DigitPos_HH_1);

	ClockHHMM_add_to_buffer_colon(display);

	ClockHHMM_add_to_buffer_digit_pixels(display, digits_pixels[m0],
			digits_pixels_num[m0], DigitPos_MM_0);
	ClockHHMM_add_to_buffer_digit_pixels(display, digits_pixels[m1],
			digits_pixels_num[m1], DigitPos_MM_1);

	SSD1306Display_draw_buffer(display);

	clock->is_dirty = false;
}

void ClockHHMM_add_to_buffer_colon(SSD1306Display *display) {
	if (!display) {
		return;
	}

	int8_t pixels_y[8] = { 8, 9, 10, 11, 20, 21, 22, 23 };

	for (int y = 0; y < 8; y++) {
		SSD1306Display_set_pixel(display, 63, pixels_y[y], true);
		SSD1306Display_set_pixel(display, 64, pixels_y[y], true);
		SSD1306Display_set_pixel(display, 65, pixels_y[y], true);
		SSD1306Display_set_pixel(display, 66, pixels_y[y], true);
	}
}

void ClockHHMM_add_to_buffer_digit_pixels(SSD1306Display *display,
		const uint8_t pixels[][2], uint16_t pixels_num, EDigitPos digit_pos) {
	if (!display) {
		return;
	}

	uint8_t x, y, x_offset;

	switch (digit_pos) {
	case DigitPos_HH_0:
		x_offset = 10;
		break;
	case DigitPos_HH_1:
		x_offset = 10 + 24;
		break;
	case DigitPos_MM_0:
		x_offset = 10 + 24 + 24 + 12;
		break;
	case DigitPos_MM_1:
		x_offset = 10 + 24 + 24 + 12 + 24;
		break;

	}

	for (uint16_t i = 0; i < pixels_num; i++) {
		x = pixels[i][0] + x_offset;
		y = pixels[i][1];

		SSD1306Display_set_pixel(display, x, y, true);

	}
}
