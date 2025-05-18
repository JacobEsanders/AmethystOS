#include <video.h>

static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;
static uint8_t current_color = 0;
static size_t cursor_x = 0;
static size_t cursor_y = 0;

static uint8_t make_color(uint8_t fg, uint8_t bg) {
    return fg | (bg << 4);
}

static uint16_t make_vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

void init_video(void) {
    current_color = make_color(VGA_COLOR_LIGHT_PURPLE, VGA_COLOR_BLACK);
    clear_screen();
}

void clear_screen(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_MEMORY[index] = make_vga_entry(' ', current_color);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void print_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= VGA_HEIGHT) {
                // TODO: Implement scrolling
                cursor_y = 0;
            }
            continue;
        }

        const size_t index = cursor_y * VGA_WIDTH + cursor_x;
        VGA_MEMORY[index] = make_vga_entry(str[i], current_color);
        
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= VGA_HEIGHT) {
                // TODO: Implement scrolling
                cursor_y = 0;
            }
        }
    }
}

void set_color(uint8_t fg, uint8_t bg) {
    current_color = make_color(fg, bg);
}
