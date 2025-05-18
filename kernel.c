#include <kernel.h>
#include <video.h>
#include <shell.h>
#include <stddef.h>
#include <stdint.h>

/* Kernel entry point */
void kmain(void) {
    /* Initialize video first for debugging output */
    init_video();
    
    /* Initialize core systems */
    init_system();
    init_memory();
    init_process();

    /* Display welcome message with amethyst theme */
    const char* welcome = "Welcome to AmethystOS\n"
                         "Version: " AMETHYST_VERSION "\n"
                         "---------------------------\n"
                         "Initializing system components...";
    print_string(welcome);

    /* Initialize and start the shell */
    init_shell();
    shell_main();

    /* Main kernel loop */
    while(1) {
        /* TODO: Implement process scheduling and system calls */
        __asm__ volatile("hlt");
    }
}
