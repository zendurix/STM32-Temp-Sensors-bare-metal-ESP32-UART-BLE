#include "images_mode.h"

// memcpy
#include <string.h>

#include "ssd1306.h"
#include "resources/images.h"

void ImagesMode_init(ImagesMode *images_mode) {
	if (!images_mode) {
		return;
	}

	images_mode->image_index = 0;
	images_mode->is_dirty = true;
}

void ImagesMode_next_image(ImagesMode *images_mode) {
	if (!images_mode) {
		return;
	}

	images_mode->image_index++;
	if (images_mode->image_index >= IMAGES_COUNT) {
		images_mode->image_index = 0;
	}
	images_mode->is_dirty = true;
}

void ImagesMode_draw_image(ImagesMode *images_mode, SSD1306Display *display) {
	if (!display || !images_mode || !images_mode->is_dirty) {
		return;
	}

	SSD1306Display_clear_buffer(display);

	for (uint16_t page_num = 0; page_num < SSD1306_PAGES_NUM; page_num++) {
		memcpy(display->pages[page_num].buffer,
				&images[images_mode->image_index][page_num
						* SSD1306_PAGE_BYTE_SIZE],
				SSD1306_PAGE_BYTE_SIZE);
		display->pages[page_num].is_dirty = true;
		display->pages[page_num].is_empty = false;
	}

	SSD1306Display_draw_buffer(display);
}
