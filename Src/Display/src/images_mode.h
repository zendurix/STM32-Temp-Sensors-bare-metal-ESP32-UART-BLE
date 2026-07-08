#ifndef DISPLAY_SRC_IMAGES_MODE_H
#define DISPLAY_SRC_IMAGES_MODE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct SSD1306Display SSD1306Display;

typedef struct ImagesMode {
	uint8_t image_index;

	bool is_dirty;

} ImagesMode;

void ImagesMode_init(ImagesMode *images_mode);

void ImagesMode_next_image(ImagesMode *images_mode);

void ImagesMode_draw_image(ImagesMode *images_mode, SSD1306Display *display);

#endif /* DISPLAY_SRC_IMAGES_MODE_H */
