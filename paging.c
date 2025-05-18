#include <memory.h>
#include <video.h>

/* Page directory and first page table */
uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t first_page_table[1024] __attribute__((aligned(4096)));

/* Assembly functions defined in paging.asm */
extern void enable_paging(uint32_t* page_directory);
extern void load_page_directory(uint32_t* page_directory);

void init_paging(void) {
    /* Clear page directory */
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002; /* Supervisor, read/write, not present */
    }

    /* Identity map first 4MB */
    for (uint32_t i = 0; i < 1024; i++) {
        first_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITE;
    }

    /* Add first page table to directory */
    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_WRITE;
    
    /* Map kernel pages */
    uint32_t kernel_pages = (KERNEL_VIRTUAL_BASE + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint32_t i = 0; i < kernel_pages; i++) {
        map_page((void*)(i * PAGE_SIZE), (void*)(KERNEL_VIRTUAL_BASE + i * PAGE_SIZE), PAGE_PRESENT | PAGE_WRITE);
    }

    /* Load page directory and enable paging */
    load_page_directory(page_directory);
    enable_paging(page_directory);

    print_string("Paging initialized\n");
}

void map_page(void* physical, void* virtual, uint32_t flags) {
    uint32_t pd_index = (uint32_t)virtual >> 22;
    uint32_t pt_index = ((uint32_t)virtual >> 12) & 0x3FF;

    /* Check if page table exists */
    if (!(page_directory[pd_index] & PAGE_PRESENT)) {
        /* Create new page table */
        uint32_t* new_table = kmalloc(PAGE_SIZE);
        if (!new_table) return;

        /* Clear new table */
        for (int i = 0; i < 1024; i++) {
            new_table[i] = 0x00000002;
        }

        /* Add to directory */
        page_directory[pd_index] = ((uint32_t)new_table) | PAGE_PRESENT | PAGE_WRITE;
    }

    /* Get page table */
    uint32_t* page_table = (uint32_t*)(page_directory[pd_index] & ~0xFFF);
    
    /* Map the page */
    page_table[pt_index] = ((uint32_t)physical) | flags;
}

void unmap_page(void* virtual) {
    uint32_t pd_index = (uint32_t)virtual >> 22;
    uint32_t pt_index = ((uint32_t)virtual >> 12) & 0x3FF;

    if (page_directory[pd_index] & PAGE_PRESENT) {
        uint32_t* page_table = (uint32_t*)(page_directory[pd_index] & ~0xFFF);
        page_table[pt_index] = 0x00000002;
    }
}
