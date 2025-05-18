#include <process.h>
#include <memory.h>
#include <video.h>
#include <string.h>

#define MAX_PROCESSES 64
#define QUANTUM_MS 10

/* Process table */
static process_t process_table[MAX_PROCESSES];
static uint32_t next_pid = 1;
process_t* current_process = NULL;
static process_t* ready_queue = NULL;

/* Initialize process manager */
void init_process_manager(void) {
    /* Clear process table */
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].pid = 0;
        process_table[i].state = PROCESS_STATE_TERMINATED;
    }
    
    print_string("Process manager initialized\n");
}

/* Helper function to add process to ready queue */
static void enqueue_process(process_t* process) {
    if (!ready_queue) {
        ready_queue = process;
        process->next = NULL;
    } else {
        process_t* current = ready_queue;
        while (current->next) {
            current = current->next;
        }
        current->next = process;
        process->next = NULL;
    }
}

/* Helper function to remove process from ready queue */
static process_t* dequeue_process(void) {
    if (!ready_queue) return NULL;
    
    process_t* process = ready_queue;
    ready_queue = ready_queue->next;
    process->next = NULL;
    return process;
}

/* Create a new process */
process_t* create_process(const char* name, void (*entry)(void), uint32_t stack_size) {
    /* Find free process slot */
    process_t* process = NULL;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == PROCESS_STATE_TERMINATED) {
            process = &process_table[i];
            break;
        }
    }
    
    if (!process) return NULL;

    /* Initialize process control block */
    process->pid = next_pid++;
    strncpy(process->name, name, 31);
    process->name[31] = '\0';
    process->state = PROCESS_STATE_READY;
    
    /* Allocate and set up stack */
    process->stack_size = stack_size;
    process->stack = kmalloc(stack_size);
    if (!process->stack) {
        process->state = PROCESS_STATE_TERMINATED;
        return NULL;
    }

    /* Set up initial context */
    context_t* ctx = &process->context;
    ctx->eip = (uint32_t)entry;
    ctx->esp = (uint32_t)process->stack + stack_size - 4;
    ctx->ebp = ctx->esp;
    ctx->eflags = 0x200; /* Enable interrupts */
    ctx->cr3 = (uint32_t)page_directory; /* Use kernel page directory for now */

    /* Add to ready queue */
    enqueue_process(process);

    return process;
}

/* Terminate a process */
void terminate_process(process_t* process) {
    if (!process) return;
    
    /* Free resources */
    kfree(process->stack);
    
    /* Mark as terminated */
    process->state = PROCESS_STATE_TERMINATED;
    process->pid = 0;
    
    /* If this is the current process, force a schedule */
    if (process == current_process) {
        schedule();
    }
}

/* Context switch assembly function (to be defined) */
extern void context_switch(context_t* old_context, context_t* new_context);

/* Schedule next process */
void schedule(void) {
    if (!ready_queue) return;

    /* Save current process if exists */
    if (current_process) {
        if (current_process->state == PROCESS_STATE_RUNNING) {
            current_process->state = PROCESS_STATE_READY;
            enqueue_process(current_process);
        }
    }

    /* Get next process */
    process_t* next = dequeue_process();
    if (!next) return;

    /* Switch contexts */
    process_t* prev = current_process;
    current_process = next;
    current_process->state = PROCESS_STATE_RUNNING;

    if (prev) {
        context_switch(&prev->context, &next->context);
    } else {
        /* First process, just load context */
        context_switch(NULL, &next->context);
    }
}
