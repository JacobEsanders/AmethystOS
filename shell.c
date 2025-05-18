#include <shell.h>
#include <video.h>
#include <process.h>
#include <memory.h>
#include <string.h>
#include <keyboard.h>

#define HISTORY_SIZE 32
#define TAB_COMPLETE_MAX 10

/* Command table */
static const shell_command_t commands[] = {
    {"help", cmd_help, "Display this help message"},
    {"clear", cmd_clear, "Clear the screen"},
    {"echo", cmd_echo, "Print arguments to the screen"},
    {"ps", cmd_ps, "List running processes"},
    {"kill", cmd_kill, "Terminate a process"},
    {"meminfo", cmd_meminfo, "Display memory usage information"},
    {"color", cmd_color, "Change shell color theme"},
    {"ls", cmd_ls, "List directory contents"},
    {"cd", cmd_cd, "Change current directory"},
    {"pwd", cmd_pwd, "Print working directory"},
    {"mkdir", cmd_mkdir, "Create a directory"},
    {"rmdir", cmd_rmdir, "Remove a directory"},
    {"touch", cmd_touch, "Create an empty file"},
    {"rm", cmd_rm, "Remove a file"},
    {"amy", cmd_amy, "Package manager"},
    {NULL, NULL, NULL} /* Terminator */
};

/* Shell state */
static uint8_t shell_fg_color = VGA_COLOR_LIGHT_PURPLE;
static uint8_t shell_bg_color = VGA_COLOR_BLACK;
static char history[HISTORY_SIZE][SHELL_BUFFER_SIZE];
static int history_count = 0;
static int history_position = 0;

/* Command line editing */
static char current_line[SHELL_BUFFER_SIZE];
static int cursor_position = 0;
static int line_length = 0;

/* Find command completions */
static int find_completions(const char* prefix, const char** completions, int max_completions) {
    int count = 0;
    size_t prefix_len = strlen(prefix);
    
    /* Check built-in commands */
    for (const shell_command_t* cmd = commands; cmd->name && count < max_completions; cmd++) {
        if (strncmp(prefix, cmd->name, prefix_len) == 0) {
            completions[count++] = cmd->name;
        }
    }
    
    return count;
}

/* Clear current line */
static void clear_line(void) {
    while (cursor_position > 0) {
        shell_puts("\b \b");
        cursor_position--;
    }
}

/* Redraw current line */
static void redraw_line(void) {
    clear_line();
    shell_puts(current_line);
    cursor_position = line_length;
}

/* Add command to history */
static void add_to_history(const char* cmd) {
    if (strlen(cmd) == 0) return;
    
    /* Don't add if same as last command */
    if (history_count > 0 && strcmp(history[history_count - 1], cmd) == 0) {
        return;
    }
    
    if (history_count < HISTORY_SIZE) {
        strncpy(history[history_count], cmd, SHELL_BUFFER_SIZE - 1);
        history[history_count][SHELL_BUFFER_SIZE - 1] = '\0';
        history_count++;
    } else {
        /* Shift history up */
        for (int i = 0; i < HISTORY_SIZE - 1; i++) {
            strncpy(history[i], history[i + 1], SHELL_BUFFER_SIZE);
        }
        strncpy(history[HISTORY_SIZE - 1], cmd, SHELL_BUFFER_SIZE - 1);
        history[HISTORY_SIZE - 1][SHELL_BUFFER_SIZE - 1] = '\0';
    }
    history_position = history_count;
}

/* Process keyboard input */
static int process_input(char c) {
    if (c == '\n') {
        shell_putchar('\n');
        current_line[line_length] = '\0';
        add_to_history(current_line);
        return 1;
    }
    
    if (c == '\b') {
        if (cursor_position > 0) {
            memmove(&current_line[cursor_position - 1], 
                   &current_line[cursor_position],
                   line_length - cursor_position);
            cursor_position--;
            line_length--;
            redraw_line();
        }
        return 0;
    }
    
    if (c == '\t') {
        const char* completions[TAB_COMPLETE_MAX];
        int count = find_completions(current_line, completions, TAB_COMPLETE_MAX);
        
        if (count == 1) {
            /* Single completion */
            strncpy(current_line, completions[0], SHELL_BUFFER_SIZE - 1);
            line_length = strlen(current_line);
            cursor_position = line_length;
            redraw_line();
        } else if (count > 1) {
            /* Show multiple completions */
            shell_putchar('\n');
            for (int i = 0; i < count; i++) {
                shell_printf("%s  ", completions[i]);
            }
            shell_putchar('\n');
            shell_puts(PROMPT);
            shell_puts(current_line);
        }
        return 0;
    }
    
    if (c == '\x1B') {
        /* Handle arrow keys (history) */
        char seq[3];
        seq[0] = keyboard_read();
        if (seq[0] == '[') {
            seq[1] = keyboard_read();
            if (seq[1] == 'A' && history_position > 0) { /* Up */
                history_position--;
                strncpy(current_line, history[history_position], SHELL_BUFFER_SIZE);
                line_length = strlen(current_line);
                redraw_line();
            } else if (seq[1] == 'B' && history_position < history_count) { /* Down */
                history_position++;
                if (history_position < history_count) {
                    strncpy(current_line, history[history_position], SHELL_BUFFER_SIZE);
                } else {
                    current_line[0] = '\0';
                }
                line_length = strlen(current_line);
                redraw_line();
            } else if (seq[1] == 'C' && cursor_position < line_length) { /* Right */
                shell_putchar(current_line[cursor_position]);
                cursor_position++;
            } else if (seq[1] == 'D' && cursor_position > 0) { /* Left */
                shell_puts("\b");
                cursor_position--;
            }
        }
        return 0;
    }
    
    if (c >= ' ' && c <= '~' && line_length < SHELL_BUFFER_SIZE - 1) {
        memmove(&current_line[cursor_position + 1],
               &current_line[cursor_position],
               line_length - cursor_position);
        current_line[cursor_position] = c;
        cursor_position++;
        line_length++;
        redraw_line();
    }
    
    return 0;
}

void init_shell(void) {
    set_color(shell_fg_color, shell_bg_color);
    clear_screen();
    shell_printf("AmethystOS Shell v1.0 [%s theme]\n", 
                shell_fg_color == VGA_COLOR_LIGHT_PURPLE ? "default" :
                shell_fg_color == VGA_COLOR_PURPLE ? "dark" : "light");
    shell_puts("Type 'help' for a list of commands\n\n");
}

void shell_main(void) {
    while (1) {
        set_color(shell_fg_color, shell_bg_color);
        shell_puts(PROMPT);
        
        /* Reset line state */
        line_length = 0;
        cursor_position = 0;
        current_line[0] = '\0';
        
        /* Read command */
        while (1) {
            char c = keyboard_read();
            if (process_input(c)) break;
        }
        
        /* Process command */
        shell_process_command(current_line);
    }
}

void shell_process_command(char* cmd) {
    char* argv[MAX_ARGS];
    int argc = 0;
    
    /* Skip leading whitespace */
    while (*cmd && *cmd <= ' ') cmd++;
    
    /* Parse command line */
    char* token = strtok(cmd, " ");
    while (token && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    
    if (argc == 0) return;
    
    /* Search for command */
    for (const shell_command_t* cmd = commands; cmd->name; cmd++) {
        if (strcmp(argv[0], cmd->name) == 0) {
            int result = cmd->func(argc, argv);
            if (result != 0) {
                shell_printf("Command failed with error code: %d\n", result);
            }
            return;
        }
    }
    
    /* Command not found */
    shell_printf("Unknown command: %s\n", argv[0]);
    shell_puts("Type 'help' for a list of available commands\n");
}
