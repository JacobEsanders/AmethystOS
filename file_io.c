#include <file_io.h>
#include <filesystem.h>
#include <memory.h>
#include <string.h>

/* File descriptor table */
#define MAX_OPEN_FILES 32

typedef struct {
    uint32_t inode;
    uint32_t offset;
    uint8_t mode;
    uint8_t is_used;
} file_descriptor_t;

static file_descriptor_t fd_table[MAX_OPEN_FILES];

/* Initialize file I/O system */
void init_file_io(void) {
    memset(fd_table, 0, sizeof(fd_table));
}

/* Find free file descriptor */
static int get_free_fd(void) {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (!fd_table[i].is_used) {
            return i;
        }
    }
    return -1;
}

/* File operations implementation */
int file_open(const char* path, uint8_t mode) {
    uint32_t inode = path_to_inode(path);
    if (inode == FS_MAX_FILES) {
        /* Create file if it doesn't exist and we're writing */
        if (mode & (FILE_MODE_WRITE | FILE_MODE_APPEND)) {
            if (fs_create(path, FS_TYPE_FILE) == FS_SUCCESS) {
                inode = path_to_inode(path);
            }
        }
        if (inode == FS_MAX_FILES) return -1;
    }
    
    /* Check if file is already open */
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (fd_table[i].is_used && fd_table[i].inode == inode) {
            if ((fd_table[i].mode & FILE_MODE_WRITE) || (mode & FILE_MODE_WRITE)) {
                return -1; /* File already open for writing */
            }
        }
    }
    
    /* Get free file descriptor */
    int fd = get_free_fd();
    if (fd < 0) return -1;
    
    /* Initialize file descriptor */
    fd_table[fd].inode = inode;
    fd_table[fd].offset = (mode & FILE_MODE_APPEND) ? fs_inodes[inode].size : 0;
    fd_table[fd].mode = mode;
    fd_table[fd].is_used = 1;
    
    return fd;
}

int file_close(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !fd_table[fd].is_used) {
        return -1;
    }
    
    fd_table[fd].is_used = 0;
    return 0;
}

int file_read(int fd, void* buffer, size_t size) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !fd_table[fd].is_used ||
        !(fd_table[fd].mode & FILE_MODE_READ)) {
        return -1;
    }
    
    file_descriptor_t* desc = &fd_table[fd];
    fs_inode_t* inode = &fs_inodes[desc->inode];
    
    /* Check if we're trying to read past end of file */
    if (desc->offset >= inode->size) {
        return 0;
    }
    
    /* Adjust size if it would read past end of file */
    if (desc->offset + size > inode->size) {
        size = inode->size - desc->offset;
    }
    
    /* Calculate block and offset */
    uint32_t start_block = desc->offset / FS_BLOCK_SIZE;
    uint32_t block_offset = desc->offset % FS_BLOCK_SIZE;
    uint32_t bytes_remaining = size;
    uint8_t* buf_ptr = buffer;
    
    /* Find starting block */
    uint32_t current_block = inode->first_block;
    for (uint32_t i = 0; i < start_block; i++) {
        current_block = *(uint32_t*)(fs_blocks + current_block * FS_BLOCK_SIZE);
    }
    
    /* Read data */
    while (bytes_remaining > 0 && current_block < FS_MAX_BLOCKS) {
        uint32_t bytes_to_copy = FS_BLOCK_SIZE - block_offset;
        if (bytes_to_copy > bytes_remaining) {
            bytes_to_copy = bytes_remaining;
        }
        
        memcpy(buf_ptr, fs_blocks + current_block * FS_BLOCK_SIZE + block_offset,
               bytes_to_copy);
        
        bytes_remaining -= bytes_to_copy;
        buf_ptr += bytes_to_copy;
        desc->offset += bytes_to_copy;
        
        if (bytes_remaining > 0) {
            current_block = *(uint32_t*)(fs_blocks + current_block * FS_BLOCK_SIZE);
            block_offset = 0;
        }
    }
    
    return size - bytes_remaining;
}

int file_write(int fd, const void* buffer, size_t size) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !fd_table[fd].is_used ||
        !(fd_table[fd].mode & FILE_MODE_WRITE)) {
        return -1;
    }
    
    file_descriptor_t* desc = &fd_table[fd];
    fs_inode_t* inode = &fs_inodes[desc->inode];
    
    /* Calculate required blocks */
    uint32_t end_offset = desc->offset + size;
    uint32_t required_blocks = (end_offset + FS_BLOCK_SIZE - 1) / FS_BLOCK_SIZE;
    
    /* Allocate new blocks if needed */
    while (inode->block_count < required_blocks) {
        uint32_t new_block = allocate_block();
        if (new_block == FS_MAX_BLOCKS) {
            return -1;
        }
        
        if (inode->block_count == 0) {
            inode->first_block = new_block;
        } else {
            uint32_t current_block = inode->first_block;
            for (uint32_t i = 1; i < inode->block_count; i++) {
                current_block = *(uint32_t*)(fs_blocks + current_block * FS_BLOCK_SIZE);
            }
            *(uint32_t*)(fs_blocks + current_block * FS_BLOCK_SIZE) = new_block;
        }
        inode->block_count++;
    }
    
    /* Write data */
    uint32_t start_block = desc->offset / FS_BLOCK_SIZE;
    uint32_t block_offset = desc->offset % FS_BLOCK_SIZE;
    uint32_t bytes_remaining = size;
    const uint8_t* buf_ptr = buffer;
    
    uint32_t current_block = inode->first_block;
    for (uint32_t i = 0; i < start_block; i++) {
        current_block = *(uint32_t*)(fs_blocks + current_block * FS_BLOCK_SIZE);
    }
    
    while (bytes_remaining > 0) {
        uint32_t bytes_to_copy = FS_BLOCK_SIZE - block_offset;
        if (bytes_to_copy > bytes_remaining) {
            bytes_to_copy = bytes_remaining;
        }
        
        memcpy(fs_blocks + current_block * FS_BLOCK_SIZE + block_offset,
               buf_ptr, bytes_to_copy);
        
        bytes_remaining -= bytes_to_copy;
        buf_ptr += bytes_to_copy;
        desc->offset += bytes_to_copy;
        
        if (bytes_remaining > 0) {
            current_block = *(uint32_t*)(fs_blocks + current_block * FS_BLOCK_SIZE);
            block_offset = 0;
        }
    }
    
    /* Update file size if needed */
    if (desc->offset > inode->size) {
        inode->size = desc->offset;
    }
    
    return size;
}

int file_seek(int fd, int32_t offset, int whence) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !fd_table[fd].is_used) {
        return -1;
    }
    
    file_descriptor_t* desc = &fd_table[fd];
    fs_inode_t* inode = &fs_inodes[desc->inode];
    
    int32_t new_offset;
    switch (whence) {
        case SEEK_SET:
            new_offset = offset;
            break;
        case SEEK_CUR:
            new_offset = desc->offset + offset;
            break;
        case SEEK_END:
            new_offset = inode->size + offset;
            break;
        default:
            return -1;
    }
    
    if (new_offset < 0 || (uint32_t)new_offset > inode->size) {
        return -1;
    }
    
    desc->offset = new_offset;
    return new_offset;
}

/* File operations table */
file_operations_t file_ops = {
    .open = file_open,
    .close = file_close,
    .read = file_read,
    .write = file_write,
    .seek = file_seek
};
