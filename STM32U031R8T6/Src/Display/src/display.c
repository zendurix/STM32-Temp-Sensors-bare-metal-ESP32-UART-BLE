#include "../display.h"

#include "ssd1306.h"
#include "clock_mode.h"
#include "font_mode.h"
#include "images_mode.h"

typedef struct Display {
	SSD1306Display display;
	ClockHHMM clock_mode;
	FontMode font_mode;
	ImagesMode images_mode;

	EDisplayMode current_mode;

} Display;

Display* Display_create(uint8_t contrast, uint8_t hour, uint8_t minute,
		uint32_t milliseconds) {
	static Display display;

	SSD1306Display_initialize(&display.display, contrast);
	ClockHHMM_init(&display.clock_mode, hour, minute, milliseconds);
	FontMode_init(&display.font_mode);
	ImagesMode_init(&display.images_mode);

	display.current_mode = DisplayMode_CLOCK;

	return &display;
}

void Display_update(Display *display, uint32_t milliseconds) {
	if (!display) {
		return;
	}

	ClockHHMM_update(&display->clock_mode, milliseconds);

	switch (display->current_mode) {
	case DisplayMode_CLOCK:
		ClockHHMM_update_display(&display->clock_mode, &display->display);
		break;
	case DisplayMode_TEXT:
		FontMode_draw_text(&display->font_mode, &display->display);
		break;
	case DisplayMode_IMAGE:
		ImagesMode_draw_image(&display->images_mode, &display->display);
		break;
	}
}

void Display_toggle_on_off(Display *display) {
	if (!display) {
		return;
	}

	SSD1306Display_toggle_on_off(&display->display);
}

void Display_set_mode(Display *display, EDisplayMode mode) {
	if (!display) {
		return;
	}
	if (mode > 2) {
		mode = 0;
	}
	display->current_mode = mode;

	switch (display->current_mode) {
	case DisplayMode_CLOCK:
		display->clock_mode.is_dirty = true;
		break;
	case DisplayMode_TEXT:
		display->font_mode.is_dirty = true;
		break;
	case DisplayMode_IMAGE:
		display->images_mode.is_dirty = true;
		break;
	}
}

EDisplayMode Display_get_mode(Display *display) {
	if (!display) {
		return 0;
	}
	return display->current_mode;
}

void Display_switch_to_next_mode(Display *display) {
	if (!display) {
		return;
	}
	Display_set_mode(display, display->current_mode + 1);
}

void Display_set_hour_minute(Display *display, uint8_t hour, uint8_t minute) {
	if (!display) {
		return;
	}
	ClockHHMM_set_hh_mm(&display->clock_mode, hour, minute);
}

void Display_add_string_to_text(Display *display, const char *msg,
		uint8_t start_x, uint8_t start_y) {
	if (!display) {
		return;
	}
	FontMode_add_string_to_text(&display->font_mode, msg, start_x, start_y);
}

void Display_clear_text(Display *display) {
	if (!display) {
		return;
	}
	FontMode_clear_text(&display->font_mode);
}

void Display_switch_to_next_image(Display *display) {
	if (!display) {
		return;
	}
	ImagesMode_next_image(&display->images_mode);
}
