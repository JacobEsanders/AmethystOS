#ifndef _KERNEL_H_
#define _KERNEL_H_

/* System constants */
#define AMETHYST_VERSION "0.1.0"
#define MAX_PROCESSES    32
#define STACK_SIZE       4096

/* Process states */
#define PR_FREE         0
#define PR_CURR         1
#define PR_READY        2
#define PR_RECV         3
#define PR_SLEEP        4
#define PR_SUSP         5
#define PR_WAIT         6
#define PR_RECTIM       7

/* Color theme constants */
#define AMETHYST_PURPLE 0x9966CC
#define AMETHYST_DARK   0x663399
#define AMETHYST_LIGHT  0xCC99FF

/* Function prototypes */
void kmain(void);
void init_system(void);
void init_memory(void);
void init_process(void);

#endif /* _KERNEL_H_ */
