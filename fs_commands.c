#include <shell.h>
#include <filesystem.h>

/* Maximum path buffer size */
#define PATH_BUF_SIZE FS_MAX_PATH

int cmd_ls(int argc, char* argv[]) {
    char buffer[4096];
    const char* path = (argc > 1) ? argv[1] : NULL;
    
    int result = fs_list_dir(path, buffer, sizeof(buffer));
    if (result != FS_SUCCESS) {
        switch (result) {
            case FS_ERROR_NOT_FOUND:
                shell_printf("ls: directory not found: %s\n", path);
                break;
            case FS_ERROR_INVALID:
                shell_printf("ls: not a directory: %s\n", path);
                break;
            default:
                shell_puts("ls: unknown error\n");
        }
        return 1;
    }
    
    shell_puts(buffer);
    return 0;
}

int cmd_cd(int argc, char* argv[]) {
    if (argc != 2) {
        shell_puts("Usage: cd <directory>\n");
        return 1;
    }
    
    int result = fs_change_dir(argv[1]);
    if (result != FS_SUCCESS) {
        switch (result) {
            case FS_ERROR_NOT_FOUND:
                shell_printf("cd: directory not found: %s\n", argv[1]);
                break;
            case FS_ERROR_INVALID:
                shell_printf("cd: not a directory: %s\n", argv[1]);
                break;
            default:
                shell_puts("cd: unknown error\n");
        }
        return 1;
    }
    
    return 0;
}

int cmd_pwd(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    char buffer[PATH_BUF_SIZE];
    int result = fs_get_current_dir(buffer, sizeof(buffer));
    if (result != FS_SUCCESS) {
        shell_puts("pwd: error getting current directory\n");
        return 1;
    }
    
    shell_printf("%s\n", buffer);
    return 0;
}

int cmd_mkdir(int argc, char* argv[]) {
    if (argc != 2) {
        shell_puts("Usage: mkdir <directory>\n");
        return 1;
    }
    
    int result = fs_mkdir(argv[1]);
    if (result != FS_SUCCESS) {
        switch (result) {
            case FS_ERROR_EXISTS:
                shell_printf("mkdir: directory already exists: %s\n", argv[1]);
                break;
            case FS_ERROR_INVALID:
                shell_printf("mkdir: invalid directory name: %s\n", argv[1]);
                break;
            case FS_ERROR_FULL:
                shell_puts("mkdir: file system is full\n");
                break;
            default:
                shell_puts("mkdir: unknown error\n");
        }
        return 1;
    }
    
    return 0;
}

int cmd_rmdir(int argc, char* argv[]) {
    if (argc != 2) {
        shell_puts("Usage: rmdir <directory>\n");
        return 1;
    }
    
    int result = fs_rmdir(argv[1]);
    if (result != FS_SUCCESS) {
        switch (result) {
            case FS_ERROR_NOT_FOUND:
                shell_printf("rmdir: directory not found: %s\n", argv[1]);
                break;
            case FS_ERROR_INVALID:
                shell_printf("rmdir: not a directory or directory not empty: %s\n", argv[1]);
                break;
            default:
                shell_puts("rmdir: unknown error\n");
        }
        return 1;
    }
    
    return 0;
}

int cmd_touch(int argc, char* argv[]) {
    if (argc != 2) {
        shell_puts("Usage: touch <file>\n");
        return 1;
    }
    
    int result = fs_create(argv[1], FS_TYPE_FILE);
    if (result != FS_SUCCESS) {
        switch (result) {
            case FS_ERROR_EXISTS:
                shell_printf("touch: file already exists: %s\n", argv[1]);
                break;
            case FS_ERROR_INVALID:
                shell_printf("touch: invalid file name: %s\n", argv[1]);
                break;
            case FS_ERROR_FULL:
                shell_puts("touch: file system is full\n");
                break;
            default:
                shell_puts("touch: unknown error\n");
        }
        return 1;
    }
    
    return 0;
}

int cmd_rm(int argc, char* argv[]) {
    if (argc != 2) {
        shell_puts("Usage: rm <file>\n");
        return 1;
    }
    
    int result = fs_delete(argv[1]);
    if (result != FS_SUCCESS) {
        switch (result) {
            case FS_ERROR_NOT_FOUND:
                shell_printf("rm: file not found: %s\n", argv[1]);
                break;
            case FS_ERROR_INVALID:
                shell_printf("rm: is a directory: %s\n", argv[1]);
                break;
            default:
                shell_puts("rm: unknown error\n");
        }
        return 1;
    }
    
    return 0;
}
