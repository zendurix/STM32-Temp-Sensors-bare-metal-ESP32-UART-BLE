#ifndef DISPLAY_SRC_CLOCK_MODE_H
#define DISPLAY_SRC_CLOCK_MODE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct SSD1306Display SSD1306Display;

typedef enum EDigitPos {
	DigitPos_HH_0 = 0, //
	DigitPos_HH_1, //
	DigitPos_MM_0, //
	DigitPos_MM_1, //
} EDigitPos;

typedef struct ClockHHMM {
	uint8_t hour;
	uint8_t minute;

	uint32_t last_minute_time_stamp;
	bool is_dirty;

} ClockHHMM;

void ClockHHMM_init(ClockHHMM *clock, uint8_t hour, uint8_t minute,
		uint32_t milliseconds);

void ClockHHMM_set_hh_mm(ClockHHMM *clock, uint8_t hour, uint8_t minute);

void ClockHHMM_update(ClockHHMM *clock, uint32_t milliseconds);

void ClockHHMM_update_display(ClockHHMM *clock, SSD1306Display *display);

void ClockHHMM_add_to_buffer_colon(SSD1306Display *display);

void ClockHHMM_add_to_buffer_digit_pixels(SSD1306Display *display,
		const uint8_t pixels[][2], uint16_t pixels_num, EDigitPos digit_pos);

#endif /* DISPLAY_SRC_CLOCK_MODE_H*/
