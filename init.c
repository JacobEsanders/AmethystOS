#include <kernel.h>
#include <keyboard.h>
#include <memory.h>
#include <process.h>
#include <video.h>
#include <stddef.h>

void init_system(void) {
    /* Initialize hardware-specific features */
    init_keyboard();
    
    /* Set up interrupt handlers */
    // TODO: Implement IDT and interrupt handlers
    
    /* Initialize system clock */
    // TODO: Implement PIT configuration
    
    print_string("Keyboard initialized\n");
}

void init_memory(void) {
    /* Initialize physical memory manager with 64MB of RAM for testing */
    init_physical_memory(64 * 1024 * 1024);
    
    /* Initialize paging system */
    init_paging();
}

void init_process(void) {
    init_process_manager();
    
    /* Create initial system process */
    process_t* init = create_process("init", NULL, 4096);
    if (!init) {
        print_string("Failed to create initial process\n");
        return;
    }
    
    print_string("Initial process created\n");
}
