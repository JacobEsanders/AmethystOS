#include <filesystem.h>
#include <memory.h>
#include <stddef.h>
#include <video.h>
#include <string.h>

/* File system structures in memory */
uint8_t* fs_blocks;
fs_inode_t* fs_inodes;
uint8_t* block_bitmap;
fs_file_handle_t open_files[FS_MAX_OPEN_FILES];
uint32_t current_directory;

/* Initialize file system */
void init_filesystem(void) {
    /* Allocate memory for file system structures */
    fs_blocks = kmalloc(FS_BLOCK_SIZE * FS_MAX_BLOCKS);
    fs_inodes = kmalloc(sizeof(fs_inode_t) * FS_MAX_FILES);
    block_bitmap = kmalloc(FS_MAX_BLOCKS / 8);
    
    if (!fs_blocks || !fs_inodes || !block_bitmap) {
        print_string("Failed to allocate file system memory\n");
        return;
    }
    
    /* Clear all structures */
    memset(fs_blocks, 0, FS_BLOCK_SIZE * FS_MAX_BLOCKS);
    memset(fs_inodes, 0, sizeof(fs_inode_t) * FS_MAX_FILES);
    memset(block_bitmap, 0, FS_MAX_BLOCKS / 8);
    memset(open_files, 0, sizeof(open_files));
    
    /* Create root directory */
    fs_format();
    print_string("File system initialized\n");
}

/* Format file system */
int fs_format(void) {
    /* Clear all structures */
    memset(fs_blocks, 0, FS_BLOCK_SIZE * FS_MAX_BLOCKS);
    memset(fs_inodes, 0, sizeof(fs_inode_t) * FS_MAX_FILES);
    memset(block_bitmap, 0, FS_MAX_BLOCKS / 8);
    
    /* Create root directory */
    fs_inode_t* root = &fs_inodes[0];
    strcpy(root->name, "/");
    root->type = FS_TYPE_DIRECTORY;
    root->permissions = FS_PERM_READ | FS_PERM_WRITE | FS_PERM_EXECUTE;
    root->size = 0;
    root->block_count = 0;
    root->first_block = 0;
    root->parent_inode = 0;
    root->create_time = 0; // TODO: Add real time support
    root->modify_time = 0;
    
    current_directory = 0;
    return FS_SUCCESS;
}

/* Helper functions */
uint32_t allocate_block(void) {
    for (uint32_t i = 0; i < FS_MAX_BLOCKS; i++) {
        if (!(block_bitmap[i / 8] & (1 << (i % 8)))) {
            block_bitmap[i / 8] |= (1 << (i % 8));
            return i;
        }
    }
    return FS_MAX_BLOCKS;
}

void free_block(uint32_t block) {
    if (block < FS_MAX_BLOCKS) {
        block_bitmap[block / 8] &= ~(1 << (block % 8));
    }
}

static uint32_t find_free_inode(void) {
    for (uint32_t i = 1; i < FS_MAX_FILES; i++) {
        if (fs_inodes[i].type == 0) {
            return i;
        }
    }
    return FS_MAX_FILES;
}

uint32_t path_to_inode(const char* path) {
    if (!path || !*path) return FS_MAX_FILES;
    
    if (strcmp(path, "/") == 0) return 0;
    
    char temp_path[FS_MAX_PATH];
    strncpy(temp_path, path, FS_MAX_PATH - 1);
    temp_path[FS_MAX_PATH - 1] = '\0';
    
    uint32_t current = current_directory;
    char* token = strtok(temp_path, "/");
    
    while (token) {
        if (strcmp(token, ".") == 0) {
            /* Current directory */
        } else if (strcmp(token, "..") == 0) {
            /* Parent directory */
            current = fs_inodes[current].parent_inode;
        } else {
            /* Search in current directory */
            int found = 0;
            for (uint32_t i = 0; i < FS_MAX_FILES; i++) {
                if (fs_inodes[i].type != 0 && 
                    fs_inodes[i].parent_inode == current &&
                    strcmp(fs_inodes[i].name, token) == 0) {
                    current = i;
                    found = 1;
                    break;
                }
            }
            if (!found) return FS_MAX_FILES;
        }
        token = strtok(NULL, "/");
    }
    
    return current;
}

/* File operations */
int fs_create(const char* path, uint8_t type) {
    if (!path || !*path) return FS_ERROR_INVALID;
    
    /* Check if file already exists */
    if (path_to_inode(path) != FS_MAX_FILES) {
        return FS_ERROR_EXISTS;
    }
    
    /* Find last separator to get parent directory and filename */
    const char* last_slash = strrchr(path, '/');
    const char* filename = last_slash ? last_slash + 1 : path;
    
    /* Get parent directory inode */
    uint32_t parent;
    if (last_slash) {
        char parent_path[FS_MAX_PATH];
        size_t parent_len = last_slash - path;
        if (parent_len == 0) parent_len = 1; /* Root directory */
        
        strncpy(parent_path, path, parent_len);
        parent_path[parent_len] = '\0';
        
        parent = path_to_inode(parent_path);
        if (parent == FS_MAX_FILES) return FS_ERROR_NOT_FOUND;
    } else {
        parent = current_directory;
    }
    
    /* Allocate new inode */
    uint32_t inode = find_free_inode();
    if (inode == FS_MAX_FILES) return FS_ERROR_FULL;
    
    /* Initialize inode */
    fs_inode_t* new_inode = &fs_inodes[inode];
    strncpy(new_inode->name, filename, FS_MAX_FILENAME - 1);
    new_inode->name[FS_MAX_FILENAME - 1] = '\0';
   
    new_inode->permissions = FS_PERM_READ | FS_PERM_WRITE;
    if (type == FS_TYPE_DIRECTORY) {
        new_inode->permissions |= FS_PERM_EXECUTE;
    }
    new_inode->size = 0;
    new_inode->block_count = 0;
    new_inode->first_block = 0;
    new_inode->parent_inode = parent;
    new_inode->create_time = 0; // TODO: Add real time support
    new_inode->modify_time = 0;
    
    return FS_SUCCESS;
}

int fs_delete(const char* path) {
    uint32_t inode = path_to_inode(path);
    if (inode == FS_MAX_FILES) return FS_ERROR_NOT_FOUND;
    
    fs_inode_t* node = &fs_inodes[inode];
    
    /* Free all blocks */
    uint32_t current_block = node->first_block;
    for (uint32_t i = 0; i < node->block_count; i++) {
        free_block(current_block);
        /* Get next block from block list */
        current_block = *(uint32_t*)(fs_blocks + current_block * FS_BLOCK_SIZE);
    }
    
    /* Clear inode */
    memset(node, 0, sizeof(fs_inode_t));
    
    return FS_SUCCESS;
}

/* Directory operations */
int fs_mkdir(const char* path) {
    return fs_create(path, FS_TYPE_DIRECTORY);
}

int fs_rmdir(const char* path) {
    uint32_t inode = path_to_inode(path);
    if (inode == FS_MAX_FILES) return FS_ERROR_NOT_FOUND;
    
    fs_inode_t* node = &fs_inodes[inode];
    if (node->type != FS_TYPE_DIRECTORY) return FS_ERROR_INVALID;
    
    /* Check if directory is empty */
    for (uint32_t i = 0; i < FS_MAX_FILES; i++) {
        if (fs_inodes[i].type != 0 && fs_inodes[i].parent_inode == inode) {
            return FS_ERROR_INVALID; /* Directory not empty */
        }
    }
    
    return fs_delete(path);
}

int fs_list_dir(const char* path, char* buffer, size_t size) {
    uint32_t inode = path ? path_to_inode(path) : current_directory;
    if (inode == FS_MAX_FILES) return FS_ERROR_NOT_FOUND;
    
    fs_inode_t* node = &fs_inodes[inode];
    if (node->type != FS_TYPE_DIRECTORY) return FS_ERROR_INVALID;
    
    size_t offset = 0;
    
    /* List all entries in directory */
    for (uint32_t i = 0; i < FS_MAX_FILES; i++) {
        if (fs_inodes[i].type != 0 && fs_inodes[i].parent_inode == inode) {
            const char* type_str = (fs_inodes[i].type == FS_TYPE_DIRECTORY) ? "DIR " : "FILE";
            int written = snprintf(buffer + offset, size - offset,
                                 "%s %s %u bytes\n",
                                 type_str, fs_inodes[i].name, fs_inodes[i].size);
            if (written < 0 || (size_t)written >= size - offset) break;
            offset += written;
        }
    }
    
    return FS_SUCCESS;
}

/* Path operations */
int fs_change_dir(const char* path) {
    uint32_t inode = path_to_inode(path);
    if (inode == FS_MAX_FILES) return FS_ERROR_NOT_FOUND;
    
    if (fs_inodes[inode].type != FS_TYPE_DIRECTORY) {
        return FS_ERROR_INVALID;
    }
    
    current_directory = inode;
    return FS_SUCCESS;
}
