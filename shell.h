#ifndef _SHELL_H_
#define _SHELL_H_

#include <stddef.h>

#define SHELL_BUFFER_SIZE 256
#define MAX_ARGS 16
#define PROMPT "amethyst> "
#define HISTORY_SIZE 32
#define ESC_SEQ_MAX_LEN 4

/* Command function type */
typedef int (*command_func_t)(int argc, char* argv[]);

/* Command structure */
typedef struct {
    const char* name;
    command_func_t func;
    const char* help;
} shell_command_t;

/* Shell functions */
void init_shell(void);
void shell_main(void);
void shell_process_command(char* cmd);

/* Input/Output functions */
void shell_puts(const char* str);
void shell_printf(const char* format, ...);
char shell_getchar(void);
void shell_readline(char* buffer, size_t size);

/* Built-in commands */
int cmd_help(int argc, char* argv[]);
int cmd_clear(int argc, char* argv[]);
int cmd_echo(int argc, char* argv[]);
int cmd_ps(int argc, char* argv[]);
int cmd_kill(int argc, char* argv[]);
int cmd_meminfo(int argc, char* argv[]);
int cmd_color(int argc, char* argv[]);
int cmd_amy(int argc, char* argv[]);  /* Package manager */

/* File system commands */
int cmd_ls(int argc, char* argv[]);
int cmd_cd(int argc, char* argv[]);
int cmd_pwd(int argc, char* argv[]);
int cmd_mkdir(int argc, char* argv[]);
int cmd_rmdir(int argc, char* argv[]);
int cmd_touch(int argc, char* argv[]);
int cmd_rm(int argc, char* argv[]);

#endif /* _SHELL_H_ */
