#ifndef DISPLAY_DISPLAY_H
#define DISPLAY_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Display Display;

typedef enum EDisplayMode {
	DisplayMode_CLOCK = 0, //
	DisplayMode_TEXT = 1, //
	DisplayMode_IMAGE = 2, //
} EDisplayMode;

Display* Display_create(uint8_t contrast, uint8_t hour, uint8_t minute,
		uint32_t milliseconds);

void Display_update(Display *display, uint32_t milliseconds);

void Display_toggle_on_off(Display *display);

void Display_set_mode(Display *display, EDisplayMode mode);
EDisplayMode Display_get_mode(Display *display);
void Display_switch_to_next_mode(Display *display);

void Display_set_hour_minute(Display *display, uint8_t hour, uint8_t minute);

void Display_clear_text(Display *display);
void Display_add_string_to_text(Display *display, const char *msg,
		uint8_t start_x, uint8_t start_y);

void Display_switch_to_next_image(Display *display);

#endif /* DISPLAY_DISPLAY_H */
