#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stddef.h>
#include <stdint.h>

/* Memory constants */
#define PAGE_SIZE           4096
#define KERNEL_START        0x100000
#define KERNEL_VIRTUAL_BASE 0xC0000000

/* Page flags */
#define PAGE_PRESENT     0x1
#define PAGE_WRITE       0x2
#define PAGE_USER        0x4

/* Memory bitmap constants */
#define BLOCKS_PER_BYTE    8
#define BLOCK_SIZE         4096
#define BLOCK_ALIGN        BLOCK_SIZE

/* Paging structures */
extern uint32_t page_directory[1024];

/* Memory manager functions */
void init_physical_memory(uint32_t mem_upper);
void* kmalloc(size_t size);
void kfree(void* ptr);

/* Page directory functions */
void init_paging(void);
void switch_page_directory(uint32_t* page_directory);
void map_page(void* physical, void* virtual, uint32_t flags);
void unmap_page(void* virtual);

#endif /* _MEMORY_H_ */
