#include <keyboard.h>
#include <video.h>

/* US QWERTY keyboard layout - scancode to ASCII mapping */
static const char scancode_to_ascii[] = {
    0,   /* Error */
    27,  /* Escape */
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', /* Backspace */
    '\t', /* Tab */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   /* Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
    '`',
    0,   /* Left shift */
    '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,   /* Right shift */
    '*',
    0,   /* Alt */
    ' ', /* Space */
    0,   /* Caps lock */
    0,   /* F1 */
    0,   /* F2 */
    0,   /* F3 */
    0,   /* F4 */
    0,   /* F5 */
    0,   /* F6 */
    0,   /* F7 */
    0,   /* F8 */
    0,   /* F9 */
    0    /* F10 */
};

/* Shifted characters */
static const char scancode_to_ascii_shifted[] = {
    0,   /* Error */
    27,  /* Escape */
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    '\b', /* Backspace */
    '\t', /* Tab */
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   /* Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"',
    '~',
    0,   /* Left shift */
    '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,   /* Right shift */
    '*',
    0,   /* Alt */
    ' ', /* Space */
    0,   /* Caps lock */
    0,   /* F1 */
    0,   /* F2 */
    0,   /* F3 */
    0,   /* F4 */
    0,   /* F5 */
    0,   /* F6 */
    0,   /* F7 */
    0,   /* F8 */
    0,   /* F9 */
    0    /* F10 */
};

/* Key states */
static uint8_t shift_pressed = 0;
static uint8_t ctrl_pressed = 0;
static uint8_t alt_pressed = 0;
static uint8_t caps_lock_on = 0;

/* Port I/O functions */
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

void init_keyboard(void) {
    /* Reset keyboard */
    outb(KEYBOARD_COMMAND_PORT, 0xFF);
    
    /* Wait for acknowledgment */
    while ((inb(KEYBOARD_STATUS_PORT) & 0x01)) {
        inb(KEYBOARD_DATA_PORT);
    }
    
    /* Set default LED state */
    keyboard_set_leds(0);
}

char keyboard_read(void) {
    static int is_extended = 0;
    
    /* Wait for key */
    while (!(inb(KEYBOARD_STATUS_PORT) & 0x01));
    
    /* Read scancode */
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    /* Handle extended scancodes */
    if (scancode == 0xE0) {
        is_extended = 1;
        return 0;
    }
    
    /* If this is the second byte of an extended scancode */
    if (is_extended) {
        is_extended = 0;
        /* Convert extended scancode to special character sequences */
        switch (scancode) {
            case KEY_UP:    return '\x1B[A';
            case KEY_DOWN:  return '\x1B[B';
            case KEY_RIGHT: return '\x1B[C';
            case KEY_LEFT:  return '\x1B[D';
            case KEY_HOME:  return '\x1B[H';
            case KEY_END:   return '\x1B[F';
            default:        return 0;
        }
    }
    
    /* Handle key release */
    if (scancode & 0x80) {
        scancode &= 0x7F;
        switch (scancode) {
            case KEY_LSHIFT:
            case KEY_RSHIFT:
                shift_pressed = 0;
                break;
            case KEY_LCTRL:
                ctrl_pressed = 0;
                break;
            case KEY_LALT:
                alt_pressed = 0;
                break;
        }
        return 0;
    }
    
    /* Handle key press */
    switch (scancode) {
        case KEY_LSHIFT:
        case KEY_RSHIFT:
            shift_pressed = 1;
            return 0;
        case KEY_LCTRL:
            ctrl_pressed = 1;
            return 0;
        case KEY_LALT:
            alt_pressed = 1;
            return 0;
        case KEY_CAPS_LOCK:
            caps_lock_on = !caps_lock_on;
            keyboard_set_leds(caps_lock_on ? LED_CAPS_LOCK : 0);
            return 0;
    }
    
    /* Convert scancode to ASCII */
    if (scancode >= sizeof(scancode_to_ascii)) return 0;
    
    char c;
    if (shift_pressed) {
        c = scancode_to_ascii_shifted[scancode];
    } else {
        c = scancode_to_ascii[scancode];
    }
    
    /* Handle caps lock */
    if (caps_lock_on && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
        c ^= 0x20; /* Toggle case */
    }
    
    return c;
}

void keyboard_set_leds(uint8_t led_state) {
    outb(KEYBOARD_DATA_PORT, 0xED);
    outb(KEYBOARD_DATA_PORT, led_state & 0x07);
}

int is_shift_pressed(void) {
    return shift_pressed;
}

int is_caps_lock_on(void) {
    return caps_lock_on;
}

int is_ctrl_pressed(void) {
    return ctrl_pressed;
}

int is_alt_pressed(void) {
    return alt_pressed;
}
