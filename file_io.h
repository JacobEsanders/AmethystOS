#ifndef _FILE_IO_H_
#define _FILE_IO_H_

#include <stdint.h>
#include <stddef.h>

/* File modes */
#define FILE_MODE_READ   0x01
#define FILE_MODE_WRITE  0x02
#define FILE_MODE_APPEND 0x04

/* Seek modes */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* File operations */
typedef struct file_operations {
    int (*open)(const char* path, uint8_t mode);
    int (*close)(int fd);
    int (*read)(int fd, void* buffer, size_t size);
    int (*write)(int fd, const void* buffer, size_t size);
    int (*seek)(int fd, int32_t offset, int whence);
} file_operations_t;

#endif /* _FILE_IO_H_ */
