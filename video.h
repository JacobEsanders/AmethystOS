#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <stdint.h>

/* VGA text mode color constants */
#define VGA_COLOR_BLACK         0
#define VGA_COLOR_PURPLE        5
#define VGA_COLOR_LIGHT_PURPLE  13
#define VGA_COLOR_WHITE         15

/* Screen dimensions */
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

/* Function prototypes */
void init_video(void);
void clear_screen(void);
void print_string(const char* str);
void set_color(uint8_t fg, uint8_t bg);

#endif /* _VIDEO_H_ */
