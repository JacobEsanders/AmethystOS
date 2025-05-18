#include <string.h>
#include <stddef.h>
#include <stdarg.h>

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

char* strcpy(char* dest, const char* src) {
    size_t i = 0;
    while ((dest[i] = src[i])) i++;
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i]; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n-- > 0) {
        if (*s1 != *s2) {
            return (unsigned char)*s1 - (unsigned char)*s2;
        }
        if (*s1 == '\0') {
            return 0;
        }
        s1++;
        s2++;
    }
    return 0;
}

void* memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }
    return dest;
}

/* Simple printf-like formatting */
int vsnprintf(char* str, size_t size, const char* format, va_list ap) {
    if (size == 0) return 0;
    
    size_t pos = 0;
    while (*format && pos < size - 1) {
        if (*format != '%') {
            str[pos++] = *format++;
            continue;
        }
        
        format++; /* Skip % */
        
        /* Handle format specifiers */
        switch (*format) {
            case 's': {
                const char* s = va_arg(ap, const char*);
                while (*s && pos < size - 1) {
                    str[pos++] = *s++;
                }
                break;
            }
            case 'd': {
                int val = va_arg(ap, int);
                if (val < 0) {
                    str[pos++] = '-';
                    val = -val;
                }
                /* Convert to string */
                char num[32];
                int i = 0;
                do {
                    num[i++] = val % 10 + '0';
                    val /= 10;
                } while (val && i < 31);
                /* Copy in reverse */
                while (--i >= 0 && pos < size - 1) {
                    str[pos++] = num[i];
                }
                break;
            }
            case 'x': {
                unsigned int val = va_arg(ap, unsigned int);
                /* Convert to hex string */
                char num[32];
                int i = 0;
                do {
                    int digit = val & 0xF;
                    num[i++] = digit < 10 ? digit + '0' : digit - 10 + 'a';
                    val >>= 4;
                } while (val && i < 31);
                /* Copy in reverse */
                while (--i >= 0 && pos < size - 1) {
                    str[pos++] = num[i];
                }
                break;
            }
            case '%':
                if (pos < size - 1) {
                    str[pos++] = '%';
                }
                break;
            default:
                /* Ignore invalid format specifiers */
                break;
        }
        format++;
    }
    
    str[pos] = '\0';
    return pos;
}

int snprintf(char* str, size_t size, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(str, size, format, ap);
    va_end(ap);
    return result;
}

/* strtok static state */
static char* strtok_next = NULL;

char* strtok(char* str, const char* delim) {
    char* token_start;
    
    /* If string is provided, start fresh */
    if (str) {
        token_start = str;
    } else {
        /* Otherwise continue from last position */
        if (!strtok_next) return NULL;
        token_start = strtok_next;
    }
    
    /* Skip leading delimiters */
    while (*token_start && strchr(delim, *token_start)) {
        token_start++;
    }
    
    /* If we hit the end, no more tokens */
    if (!*token_start) {
        strtok_next = NULL;
        return NULL;
    }
    
    /* Find end of token */
    char* token_end = token_start;
    while (*token_end && !strchr(delim, *token_end)) {
        token_end++;
    }
    
    /* If we hit a delimiter, null terminate and save next position */
    if (*token_end) {
        *token_end = '\0';
        strtok_next = token_end + 1;
    } else {
        strtok_next = NULL;
    }
    
    return token_start;
}

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) {
            return (char*)s;
        }
        s++;
    }
    return NULL;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s) {
        if (*s == (char)c) {
            last = s;
        }
        s++;
    }
    return (char*)last;
}

char* strstr(const char* haystack, const char* needle) {
    size_t needle_len = strlen(needle);
    if (!needle_len) return (char*)haystack;
    
    while (*haystack) {
        if (strncmp(haystack, needle, needle_len) == 0) {
            return (char*)haystack;
        }
        haystack++;
    }
    return NULL;
}
