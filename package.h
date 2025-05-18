#ifndef _PACKAGE_H_
#define _PACKAGE_H_

#include <stdint.h>
#include <stddef.h>

/* Package states */
#define PKG_STATE_NOT_INSTALLED 0
#define PKG_STATE_INSTALLED     1
#define PKG_STATE_UPDATING      2

/* Package manager constants */
#define PKG_MAX_NAME        32
#define PKG_MAX_VERSION     16
#define PKG_MAX_DESC        128
#define PKG_MAX_PACKAGES    256
#define PKG_DB_PATH         "/var/amy/packages.db"
#define PKG_CACHE_PATH      "/var/amy/cache"
#define PKG_INSTALL_PATH    "/usr/local"

/* Package structure */
typedef struct {
    char name[PKG_MAX_NAME];
    char version[PKG_MAX_VERSION];
    char description[PKG_MAX_DESC];
    uint8_t state;
    uint32_t size;
    uint32_t deps_count;
    char** dependencies;
} package_t;

/* Package manager functions */
void init_package_manager(void);
int pkg_install(const char* name);
int pkg_remove(const char* name);
int pkg_update(const char* name);
int pkg_list(void);
int pkg_search(const char* query);
int pkg_info(const char* name);

/* Package database functions */
int pkg_db_load(void);
int pkg_db_save(void);
package_t* pkg_db_find(const char* name);

#endif /* _PACKAGE_H_ */
