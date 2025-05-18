#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <stdint.h>

/* Process states */
typedef enum {
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

/* Process context structure */
typedef struct {
    uint32_t eax, ebx, ecx, edx;    /* General purpose registers */
    uint32_t esi, edi, ebp;         /* Index and base registers */
    uint32_t esp;                   /* Stack pointer */
    uint32_t eip;                   /* Instruction pointer */
    uint32_t eflags;                /* CPU flags */
    uint32_t cr3;                   /* Page directory base */
} context_t;

/* Process control block */
typedef struct process {
    uint32_t pid;                   /* Process ID */
    char name[32];                  /* Process name */
    process_state_t state;          /* Current state */
    context_t context;              /* CPU context */
    uint32_t* stack;                /* Kernel stack */
    uint32_t stack_size;            /* Stack size */
    struct process* next;           /* Next process in queue */
} process_t;

/* Process management functions */
void init_process_manager(void);
process_t* create_process(const char* name, void (*entry)(void), uint32_t stack_size);
void terminate_process(process_t* process);
void schedule(void);

/* Current running process */
extern process_t* current_process;

#endif /* _PROCESS_H_ */
