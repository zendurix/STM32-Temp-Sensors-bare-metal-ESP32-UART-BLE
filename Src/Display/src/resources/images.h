#ifndef DISPLAY_SRC_RESOURCES_IMAGES_H
#define DISPLAY_SRC_RESOURCES_IMAGES_H

#include <stdint.h>

/*

 Images definitions.
 All images are 128x32 - full display buffer - 512 bytes bitmaps in format readable for ssd1306
 - each byte is 8x1 vertical strip of pixels from top left, next bytes are to the right.

 Images are converted with "\python_scripts\image_converter.py" from 128x32 black and white images created in Paint.

 */

#define IMAGES_COUNT 4
extern const uint8_t *const images[IMAGES_COUNT];

extern const uint8_t image_test[512];
extern const uint8_t image_flowers[512];
extern const uint8_t image_pizza[512];
extern const uint8_t image_pixel_art_love[512];

#endif /* DISPLAY_SRC_RESOURCES_IMAGES_H */
