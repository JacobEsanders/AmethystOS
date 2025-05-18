#include <package.h>
#include <filesystem.h>
#include <memory.h>
#include <string.h>
#include <video.h>
#include <file_io.h>

/* Package database */
static package_t packages[PKG_MAX_PACKAGES];
static uint32_t package_count = 0;

/* Initialize package manager */
void init_package_manager(void) {
    /* Create package manager directories */
    fs_mkdir("/var");
    fs_mkdir("/var/amy");
    fs_mkdir("/var/amy/cache");
    fs_mkdir("/usr");
    fs_mkdir("/usr/local");
    
    /* Load package database */
    pkg_db_load();
    
    print_string("Package manager initialized\n");
}

/* Load package database */
int pkg_db_load(void) {
    int fd = fs_open(PKG_DB_PATH, FILE_MODE_READ);
    if (fd < 0) {
        /* Create empty database if it doesn't exist */
        fd = fs_open(PKG_DB_PATH, FILE_MODE_WRITE);
        if (fd < 0) return -1;
        fs_close(fd);
        return 0;
    }
    
    /* Read package count */
    fs_read(fd, &package_count, sizeof(package_count));
    
    /* Read packages */
    if (package_count > PKG_MAX_PACKAGES) {
        package_count = PKG_MAX_PACKAGES;
    }
    
    for (uint32_t i = 0; i < package_count; i++) {
        fs_read(fd, &packages[i], sizeof(package_t));
    }
    
    fs_close(fd);
    return 0;
}

/* Save package database */
int pkg_db_save(void) {
    int fd = fs_open(PKG_DB_PATH, FILE_MODE_WRITE);
    if (fd < 0) return -1;
    
    /* Write package count */
    fs_write(fd, &package_count, sizeof(package_count));
    
    /* Write packages */
    for (uint32_t i = 0; i < package_count; i++) {
        fs_write(fd, &packages[i], sizeof(package_t));
    }
    
    fs_close(fd);
    return 0;
}

/* Find package in database */
package_t* pkg_db_find(const char* name) {
    for (uint32_t i = 0; i < package_count; i++) {
        if (strcmp(packages[i].name, name) == 0) {
            return &packages[i];
        }
    }
    return NULL;
}

/* Package operations */
int pkg_install(const char* name) {
    package_t* pkg = pkg_db_find(name);
    if (!pkg) {
        /* Package not in database */
        return -1;
    }
    
    if (pkg->state == PKG_STATE_INSTALLED) {
        /* Already installed */
        return -2;
    }
    
    /* Check dependencies */
    for (uint32_t i = 0; i < pkg->deps_count; i++) {
        package_t* dep = pkg_db_find(pkg->dependencies[i]);
        if (!dep || dep->state != PKG_STATE_INSTALLED) {
            /* Missing dependency */
            return -3;
        }
    }
    
    /* Create package directory */
    char path[FS_MAX_PATH];
    snprintf(path, sizeof(path), "%s/%s", PKG_INSTALL_PATH, pkg->name);
    fs_mkdir(path);
    
    /* Mark as installed */
    pkg->state = PKG_STATE_INSTALLED;
    pkg_db_save();
    
    return 0;
}

int pkg_remove(const char* name) {
    package_t* pkg = pkg_db_find(name);
    if (!pkg) {
        /* Package not in database */
        return -1;
    }
    
    if (pkg->state != PKG_STATE_INSTALLED) {
        /* Not installed */
        return -2;
    }
    
    /* Check if other packages depend on this one */
    for (uint32_t i = 0; i < package_count; i++) {
        if (packages[i].state == PKG_STATE_INSTALLED) {
            for (uint32_t j = 0; j < packages[i].deps_count; j++) {
                if (strcmp(packages[i].dependencies[j], name) == 0) {
                    /* Package is needed by another package */
                    return -3;
                }
            }
        }
    }
    
    /* Remove package directory */
    char path[FS_MAX_PATH];
    snprintf(path, sizeof(path), "%s/%s", PKG_INSTALL_PATH, pkg->name);
    fs_rmdir(path);
    
    /* Mark as not installed */
    pkg->state = PKG_STATE_NOT_INSTALLED;
    pkg_db_save();
    
    return 0;
}

int pkg_update(const char* name) {
    package_t* pkg = pkg_db_find(name);
    if (!pkg) {
        /* Package not in database */
        return -1;
    }
    
    if (pkg->state != PKG_STATE_INSTALLED) {
        /* Not installed */
        return -2;
    }
    
    /* TODO: Check for newer version */
    
    return 0;
}

int pkg_list(void) {
    print_string("Installed packages:\n");
    print_string("------------------\n");
    
    for (uint32_t i = 0; i < package_count; i++) {
        if (packages[i].state == PKG_STATE_INSTALLED) {
            char buf[256];
            snprintf(buf, sizeof(buf), "%s (%s) - %s\n",
                    packages[i].name,
                    packages[i].version,
                    packages[i].description);
            print_string(buf);
        }
    }
    
    return 0;
}

int pkg_search(const char* query) {
    print_string("Search results:\n");
    print_string("--------------\n");
    
    for (uint32_t i = 0; i < package_count; i++) {
        if (strstr(packages[i].name, query) || strstr(packages[i].description, query)) {
            char buf[256];
            snprintf(buf, sizeof(buf), "%s (%s) [%s] - %s\n",
                    packages[i].name,
                    packages[i].version,
                    packages[i].state == PKG_STATE_INSTALLED ? "installed" : "not installed",
                    packages[i].description);
            print_string(buf);
        }
    }
    
    return 0;
}

int pkg_info(const char* name) {
    package_t* pkg = pkg_db_find(name);
    if (!pkg) {
        /* Package not in database */
        return -1;
    }
    
    char buf[512];
    snprintf(buf, sizeof(buf),
             "Package: %s\n"
             "Version: %s\n"
             "Status: %s\n"
             "Size: %u bytes\n"
             "Description: %s\n"
             "Dependencies (%u):\n",
             pkg->name,
             pkg->version,
             pkg->state == PKG_STATE_INSTALLED ? "installed" : "not installed",
             pkg->size,
             pkg->description,
             pkg->deps_count);
    print_string(buf);
    
    for (uint32_t i = 0; i < pkg->deps_count; i++) {
        snprintf(buf, sizeof(buf), "  - %s\n", pkg->dependencies[i]);
        print_string(buf);
    }
    
    return 0;
}
