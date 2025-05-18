#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <stdint.h>
#include <stddef.h>

/* File system constants */
#define FS_MAX_FILENAME    64
#define FS_MAX_PATH        256
#define FS_BLOCK_SIZE      4096
#define FS_MAX_BLOCKS      65536
#define FS_MAX_FILES       1024
#define FS_MAX_OPEN_FILES  32

/* File types */
#define FS_TYPE_FILE       1
#define FS_TYPE_DIRECTORY  2

/* File permissions */
#define FS_PERM_READ       0x04
#define FS_PERM_WRITE      0x02
#define FS_PERM_EXECUTE    0x01

/* Error codes */
#define FS_SUCCESS         0
#define FS_ERROR_NOT_FOUND -1
#define FS_ERROR_EXISTS    -2
#define FS_ERROR_FULL      -3
#define FS_ERROR_INVALID   -4
#define FS_ERROR_PERM      -5

/* File system structures */
typedef struct {
    char name[FS_MAX_FILENAME];
    uint8_t type;
    uint8_t permissions;
    uint32_t size;
    uint32_t block_count;
    uint32_t first_block;
    uint32_t parent_inode;
    uint32_t create_time;
    uint32_t modify_time;
} fs_inode_t;

typedef struct {
    uint32_t inode;
    uint32_t offset;
    uint8_t mode;  /* read/write/append */
    uint8_t flags;
} fs_file_handle_t;

/* Global filesystem state */
extern fs_inode_t* fs_inodes;
extern uint32_t current_directory;
extern uint8_t* fs_blocks;
extern uint32_t path_to_inode(const char* path);
extern uint32_t allocate_block(void);
extern void free_block(uint32_t block);

/* File system functions */
void init_filesystem(void);
int fs_format(void);

/* File operations */
int fs_create(const char* path, uint8_t type);
int fs_delete(const char* path);
int fs_open(const char* path, uint8_t mode);
int fs_close(int handle);
int fs_read(int handle, void* buffer, size_t size);
int fs_write(int handle, const void* buffer, size_t size);
int fs_seek(int handle, int32_t offset, int whence);

/* Directory operations */
int fs_mkdir(const char* path);
int fs_rmdir(const char* path);
int fs_list_dir(const char* path, char* buffer, size_t size);

/* Path operations */
int fs_get_current_dir(char* buffer, size_t size);
int fs_change_dir(const char* path);

/* Utility functions */
int fs_exists(const char* path);
int fs_get_type(const char* path);
int fs_get_size(const char* path);

#endif /* _FILESYSTEM_H_ */
