#include <shell.h>
#include <package.h>

int cmd_amy(int argc, char* argv[]) {
    if (argc < 2) {
        shell_puts("Usage: amy <command> [args]\n");
        shell_puts("Commands:\n");
        shell_puts("  install <package>  - Install a package\n");
        shell_puts("  remove <package>   - Remove a package\n");
        shell_puts("  update [package]   - Update packages\n");
        shell_puts("  list              - List installed packages\n");
        shell_puts("  search <query>    - Search for packages\n");
        shell_puts("  info <package>    - Show package information\n");
        return 1;
    }
    
    if (strcmp(argv[1], "install") == 0) {
        if (argc != 3) {
            shell_puts("Usage: amy install <package>\n");
            return 1;
        }
        
        int result = pkg_install(argv[2]);
        switch (result) {
            case 0:
                shell_printf("Package '%s' installed successfully\n", argv[2]);
                break;
            case -1:
                shell_printf("Package '%s' not found\n", argv[2]);
                break;
            case -2:
                shell_printf("Package '%s' is already installed\n", argv[2]);
                break;
            case -3:
                shell_printf("Failed to install dependencies for '%s'\n", argv[2]);
                break;
            default:
                shell_printf("Failed to install package '%s'\n", argv[2]);
        }
        return result != 0;
    }
    
    if (strcmp(argv[1], "remove") == 0) {
        if (argc != 3) {
            shell_puts("Usage: amy remove <package>\n");
            return 1;
        }
        
        int result = pkg_remove(argv[2]);
        switch (result) {
            case 0:
                shell_printf("Package '%s' removed successfully\n", argv[2]);
                break;
            case -1:
                shell_printf("Package '%s' not found\n", argv[2]);
                break;
            case -2:
                shell_printf("Package '%s' is not installed\n", argv[2]);
                break;
            case -3:
                shell_printf("Package '%s' is required by other packages\n", argv[2]);
                break;
            default:
                shell_printf("Failed to remove package '%s'\n", argv[2]);
        }
        return result != 0;
    }
    
    if (strcmp(argv[1], "update") == 0) {
        if (argc > 3) {
            shell_puts("Usage: amy update [package]\n");
            return 1;
        }
        
        if (argc == 3) {
            int result = pkg_update(argv[2]);
            switch (result) {
                case 0:
                    shell_printf("Package '%s' updated successfully\n", argv[2]);
                    break;
                case -1:
                    shell_printf("Package '%s' not found\n", argv[2]);
                    break;
                case -2:
                    shell_printf("Package '%s' is not installed\n", argv[2]);
                    break;
                default:
                    shell_printf("Failed to update package '%s'\n", argv[2]);
            }
            return result != 0;
        } else {
            shell_puts("Updating package database...\n");
            // TODO: Implement full system update
            return 0;
        }
    }
    
    if (strcmp(argv[1], "list") == 0) {
        return pkg_list();
    }
    
    if (strcmp(argv[1], "search") == 0) {
        if (argc != 3) {
            shell_puts("Usage: amy search <query>\n");
            return 1;
        }
        
        return pkg_search(argv[2]);
    }
    
    if (strcmp(argv[1], "info") == 0) {
        if (argc != 3) {
            shell_puts("Usage: amy info <package>\n");
            return 1;
        }
        
        int result = pkg_info(argv[2]);
        if (result < 0) {
            shell_printf("Package '%s' not found\n", argv[2]);
            return 1;
        }
        return 0;
    }
    
    shell_printf("Unknown command: %s\n", argv[1]);
    return 1;
}
