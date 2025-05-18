#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdint.h>

/* Keyboard ports */
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_COMMAND_PORT 0x64

/* Keyboard LED states */
#define LED_SCROLL_LOCK 0x01
#define LED_NUM_LOCK    0x02
#define LED_CAPS_LOCK   0x04

/* Special keys */
#define KEY_ESCAPE    0x01
#define KEY_BACKSPACE 0x0E
#define KEY_TAB       0x0F
#define KEY_ENTER     0x1C
#define KEY_LCTRL     0x1D
#define KEY_LSHIFT    0x2A
#define KEY_RSHIFT    0x36
#define KEY_LALT      0x38
#define KEY_CAPS_LOCK 0x3A
#define KEY_F1        0x3B
#define KEY_F2        0x3C
#define KEY_F3        0x3D
#define KEY_F4        0x3E
#define KEY_F5        0x3F
#define KEY_F6        0x40
#define KEY_F7        0x41
#define KEY_F8        0x42
#define KEY_F9        0x43
#define KEY_F10       0x44
#define KEY_F11       0x57
#define KEY_F12       0x58

/* Extended (E0) scancodes */
#define KEY_UP        0x48
#define KEY_DOWN      0x50
#define KEY_LEFT      0x4B
#define KEY_RIGHT     0x4D
#define KEY_HOME      0x47
#define KEY_END       0x4F
#define KEY_DELETE    0x53

/* Function prototypes */
void init_keyboard(void);
char keyboard_read(void);
void keyboard_set_leds(uint8_t led_state);
int is_key_pressed(uint8_t scancode);

/* Key state functions */
int is_shift_pressed(void);
int is_caps_lock_on(void);
int is_ctrl_pressed(void);
int is_alt_pressed(void);

#endif /* _KEYBOARD_H_ */
