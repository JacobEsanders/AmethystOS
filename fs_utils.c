#include <filesystem.h>
#include <string.h>

/* File system utilities */

/* Convert absolute path to relative path */
static void make_relative_path(const char* abs_path, char* rel_path, size_t size) {
    if (strcmp(abs_path, "/") == 0) {
        strncpy(rel_path, ".", size - 1);
        rel_path[size - 1] = '\0';
        return;
    }
    
    /* Skip leading slash */
    if (abs_path[0] == '/') {
        abs_path++;
    }
    
    strncpy(rel_path, abs_path, size - 1);
    rel_path[size - 1] = '\0';
}

/* Convert relative path to absolute path */
static void make_absolute_path(const char* rel_path, char* abs_path, size_t size) {
    if (rel_path[0] == '/') {
        strncpy(abs_path, rel_path, size - 1);
        abs_path[size - 1] = '\0';
        return;
    }
    
    /* Get current directory path */
    char current_path[FS_MAX_PATH];
    fs_get_current_dir(current_path, sizeof(current_path));
    
    if (strcmp(rel_path, ".") == 0) {
        strncpy(abs_path, current_path, size - 1);
        abs_path[size - 1] = '\0';
        return;
    }
    
    /* Combine paths */
    snprintf(abs_path, size, "%s/%s", current_path, rel_path);
}

/* Normalize path (remove . and .. components) */
static void normalize_path(char* path) {
    char* src = path;
    char* dst = path;
    char* last = NULL;
    char* components[FS_MAX_PATH / 2];
    int component_count = 0;
    
    /* Skip leading slash */
    if (*src == '/') {
        *dst++ = *src++;
    }
    
    while (*src) {
        if (*src == '/') {
            src++;
            continue;
        }
        
        /* Mark start of component */
        last = dst;
        components[component_count++] = dst;
        
        /* Copy until next slash or end */
        while (*src && *src != '/') {
            *dst++ = *src++;
        }
        
        /* Handle . and .. */
        if (last && dst - last == 1 && *last == '.') {
            /* Remove . component */
            dst = last;
            component_count--;
        } else if (last && dst - last == 2 && last[0] == '.' && last[1] == '.') {
            /* Remove .. and previous component */
            if (component_count >= 2) {
                dst = components[component_count - 2];
                component_count -= 2;
            }
        }
        
        if (*src && dst > path && dst[-1] != '/') {
            *dst++ = '/';
        }
    }
    
    /* Remove trailing slash unless root */
    if (dst > path + 1 && dst[-1] == '/') {
        dst--;
    }
    
    *dst = '\0';
}

/* Get parent directory path */
static void get_parent_path(const char* path, char* parent, size_t size) {
    strncpy(parent, path, size - 1);
    parent[size - 1] = '\0';
    
    char* last_slash = strrchr(parent, '/');
    if (last_slash == parent) {
        /* Root directory */
        parent[1] = '\0';
    } else if (last_slash) {
        *last_slash = '\0';
    }
}

/* Get file name from path */
static void get_file_name(const char* path, char* name, size_t size) {
    const char* last_slash = strrchr(path, '/');
    const char* filename = last_slash ? last_slash + 1 : path;
    
    strncpy(name, filename, size - 1);
    name[size - 1] = '\0';
}

/* File system API implementations */

int fs_get_current_dir(char* buffer, size_t size) {
    if (!buffer || size == 0) return FS_ERROR_INVALID;
    
    char path[FS_MAX_PATH];
    uint32_t current = current_directory;
    int path_len = 0;
    
    /* Build path from current directory up to root */
    while (current != 0) {
        fs_inode_t* node = &fs_inodes[current];
        int name_len = strlen(node->name);
        
        /* Check if we have enough space */
        if (path_len + name_len + 1 >= FS_MAX_PATH) {
            return FS_ERROR_INVALID;
        }
        
        /* Move existing path components right */
        memmove(path + name_len + 1, path, path_len);
        
        /* Add current component */
        memcpy(path, node->name, name_len);
        path[name_len] = '/';
        path_len += name_len + 1;
        
        current = node->parent_inode;
    }
    
    /* Add root slash if not root directory */
    if (path_len == 0) {
        path[path_len++] = '/';
    }
    
    path[path_len] = '\0';
    
    /* Copy to output buffer */
    if ((size_t)path_len >= size) {
        return FS_ERROR_INVALID;
    }
    
    strcpy(buffer, path);
    return FS_SUCCESS;
}

int fs_exists(const char* path) {
    return path_to_inode(path) != FS_MAX_FILES;
}

int fs_get_type(const char* path) {
    uint32_t inode = path_to_inode(path);
    if (inode == FS_MAX_FILES) return FS_ERROR_NOT_FOUND;
    
    return fs_inodes[inode].type;
}

int fs_get_size(const char* path) {
    uint32_t inode = path_to_inode(path);
    if (inode == FS_MAX_FILES) return FS_ERROR_NOT_FOUND;
    
    return fs_inodes[inode].size;
}
