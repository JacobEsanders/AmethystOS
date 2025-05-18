#include <shell.h>
#include <video.h>
#include <stdarg.h>

#define MAX_PRINTF_BUF 256

void shell_puts(const char* str) {
    print_string(str);
}

void shell_putchar(char c) {
    char str[2] = {c, '\0'};
    print_string(str);
}

void shell_printf(const char* format, ...) {
    char buffer[MAX_PRINTF_BUF];
    va_list args;
    va_start(args, format);
    
    char* buf_ptr = buffer;
    while (*format && (buf_ptr - buffer) < MAX_PRINTF_BUF - 1) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd': {
                    int val = va_arg(args, int);
                    int tmp = val;
                    char num_buf[12];
                    char* num_ptr = num_buf + 11;
                    *num_ptr = '\0';
                    
                    if (val < 0) {
                        val = -val;
                        *buf_ptr++ = '-';
                    }
                    
                    do {
                        *--num_ptr = '0' + (val % 10);
                        val /= 10;
                    } while (val);
                    
                    while (*num_ptr) {
                        *buf_ptr++ = *num_ptr++;
                    }
                    break;
                }
                case 's': {
                    char* str = va_arg(args, char*);
                    while (*str) {
                        *buf_ptr++ = *str++;
                    }
                    break;
                }
                default:
                    *buf_ptr++ = *format;
                    break;
            }
        } else {
            *buf_ptr++ = *format;
        }
        format++;
    }
    
    *buf_ptr = '\0';
    va_end(args);
    
    print_string(buffer);
}

char shell_getchar(void) {
    // TODO: Implement keyboard driver and input handling
    return '\0';
}

/* Command history */
static char history[HISTORY_SIZE][SHELL_BUFFER_SIZE];
static int history_count = 0;
static int history_position = 0;

/* Clear the current line in the terminal */
static void clear_line(size_t current_pos) {
    while (current_pos > 0) {
        shell_puts("\b \b");
        current_pos--;
    }
}

/* Handle escape sequence */
static int handle_escape_sequence(char* seq) {
    if (strcmp(seq, "[A") == 0) { /* Up arrow */
        return 1;
    } else if (strcmp(seq, "[B") == 0) { /* Down arrow */
        return 2;
    }
    return 0;
}

void shell_readline(char* buffer, size_t size) {
    size_t i = 0;
    char c;
    char esc_seq[ESC_SEQ_MAX_LEN] = {0};
    int esc_pos = 0;
    int in_escape = 0;
    int temp_history_pos = history_position;
    
    /* Clear the buffer */
    memset(buffer, 0, size);
    
    while (i < size - 1) {
        c = keyboard_read();
        if (!c) continue;
        
        if (c == '\x1B') { /* Escape sequence start */
            in_escape = 1;
            esc_pos = 0;
            continue;
        }
        
        if (in_escape) {
            if (esc_pos < ESC_SEQ_MAX_LEN - 1) {
                esc_seq[esc_pos++] = c;
                esc_seq[esc_pos] = '\0';
                
                int action = handle_escape_sequence(esc_seq);
                if (action > 0) {
                    /* Handle up/down arrows */
                    if (action == 1 && temp_history_pos > 0) { /* Up arrow */
                        temp_history_pos--;
                        clear_line(i);
                        strncpy(buffer, history[temp_history_pos], size - 1);
                        i = strlen(buffer);
                        shell_puts(buffer);
                    } else if (action == 2 && temp_history_pos < history_count) { /* Down arrow */
                        temp_history_pos++;
                        clear_line(i);
                        if (temp_history_pos < history_count) {
                            strncpy(buffer, history[temp_history_pos], size - 1);
                            i = strlen(buffer);
                            shell_puts(buffer);
                        } else {
                            buffer[0] = '\0';
                            i = 0;
                        }
                    }
                    in_escape = 0;
                }
            }
            continue;
        }
        
        if (c == '\n') {
            shell_putchar('\n');
            /* Add to history if not empty and different from last command */
            if (i > 0 && (history_count == 0 || strcmp(buffer, history[history_count - 1]) != 0)) {
                if (history_count < HISTORY_SIZE) {
                    strncpy(history[history_count], buffer, SHELL_BUFFER_SIZE - 1);
                    history_count++;
                } else {
                    /* Shift history up */
                    for (int j = 0; j < HISTORY_SIZE - 1; j++) {
                        strncpy(history[j], history[j + 1], SHELL_BUFFER_SIZE - 1);
                    }
                    strncpy(history[HISTORY_SIZE - 1], buffer, SHELL_BUFFER_SIZE - 1);
                }
                history_position = history_count;
            }
            break;
        } else if (c == '\b') {
            if (i > 0) {
                i--;
                shell_puts("\b \b"); /* Erase character */
            }
        } else {
            buffer[i++] = c;
            shell_putchar(c);
        }
    }
    
    buffer[i] = '\0';
}
