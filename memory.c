#include <memory.h>
#include <video.h>

/* Memory bitmap - each bit represents a 4KB block */
#define MEMORY_BITMAP_SIZE 32768 /* Supports up to 4GB of RAM */
static uint8_t memory_bitmap[MEMORY_BITMAP_SIZE];
static uint32_t total_blocks;
static uint32_t used_blocks;

/* Helper functions */
static void set_block(uint32_t bit) {
    memory_bitmap[bit / 8] |= (1 << (bit % 8));
    used_blocks++;
}

static void clear_block(uint32_t bit) {
    memory_bitmap[bit / 8] &= ~(1 << (bit % 8));
    used_blocks--;
}

static uint32_t test_block(uint32_t bit) {
    return memory_bitmap[bit / 8] & (1 << (bit % 8));
}

static int32_t find_free_blocks(uint32_t count) {
    if (count == 0) return -1;

    uint32_t first_free = 0;
    uint32_t free_count = 0;

    for (uint32_t i = 0; i < total_blocks; i++) {
        if (!test_block(i)) {
            if (free_count == 0) first_free = i;
            free_count++;
            if (free_count == count) return first_free;
        } else {
            free_count = 0;
        }
    }
    return -1;
}

void init_physical_memory(uint32_t mem_upper) {
    total_blocks = mem_upper / BLOCK_SIZE;
    used_blocks = 0;

    /* Mark all memory as free initially */
    for (uint32_t i = 0; i < MEMORY_BITMAP_SIZE; i++) {
        memory_bitmap[i] = 0;
    }

    /* Mark kernel space as used */
    uint32_t kernel_blocks = (KERNEL_VIRTUAL_BASE + BLOCK_SIZE - 1) / BLOCK_SIZE;
    for (uint32_t i = 0; i < kernel_blocks; i++) {
        set_block(i);
    }

    print_string("Physical memory manager initialized\n");
    print_string("Total memory: ");
    // TODO: Add function to print numbers
    print_string(" KB\n");
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;

    /* Calculate number of blocks needed */
    uint32_t blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    /* Find free block(s) */
    int32_t block_index = find_free_blocks(blocks);
    if (block_index < 0) return NULL;

    /* Mark blocks as used */
    for (uint32_t i = 0; i < blocks; i++) {
        set_block(block_index + i);
    }

    return (void*)(block_index * BLOCK_SIZE);
}

void kfree(void* ptr) {
    if (!ptr) return;

    uint32_t block_index = (uint32_t)ptr / BLOCK_SIZE;
    
    /* Find end of allocation by looking for next used block */
    while (block_index < total_blocks && test_block(block_index)) {
        clear_block(block_index);
        block_index++;
    }
}
