#include <kernel/string.h>

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

void* memset(void* ptr, uint8_t value, size_t num) {
    uint8_t* p = (uint8_t*)ptr;
    for (size_t i = 0; i < num; i++) {
        p[i] = value;
    }
    return ptr;
}

void* memcpy(void* dest, const void* src, size_t num) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < num; i++) {
        d[i] = s[i];
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t num) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    
    if (d < s) {
        for (size_t i = 0; i < num; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = num; i > 0; i--) {
            d[i-1] = s[i-1];
        }
    }
    return dest;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
    const uint8_t* p1 = (const uint8_t*)ptr1;
    const uint8_t* p2 = (const uint8_t*)ptr2;
    for (size_t i = 0; i < num; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    return 0;
}

int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const uint8_t*)str1 - *(const uint8_t*)str2;
}

char* strcpy(char* dest, const char* src) {
    char* save = dest;
    while ((*dest++ = *src++));
    return save;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* save = dest;
    while (n && (*dest++ = *src++)) n--;
    while (n--) *dest++ = '\0';
    return save;
}

size_t strlcpy(char* dest, const char* src, size_t n) {
    char* d = dest;
    const char* s = src;
    size_t len = strlen(src);
    
    if (n == 0) return len;
    
    while (--n && (*d++ = *s++));
    *d = '\0';
    
    return len;
}

char* strcat(char* dest, const char* src) {
    char* save = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return save;
}

char* strncat(char* dest, const char* src, size_t n) {
    char* save = dest;
    while (*dest) dest++;
    while (n-- && (*dest++ = *src++));
    *dest = '\0';
    return save;
}

/* 查找字符 */
char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == (char)c) {
            return (char*)str;
        }
        str++;
    }
    return NULL;
}

char* strrchr(const char* str, int c) {
    const char* last = NULL;
    while (*str) {
        if (*str == (char)c) {
            last = str;
        }
        str++;
    }
    return (char*)last;
}

/* 字符串分割 */
char* strtok_r(char* str, const char* delim, char** saveptr) {
    char* token;
    
    if (str == NULL) {
        str = *saveptr;
    }
    
    /* 跳过前导分隔符 */
    while (*str && strchr(delim, *str)) {
        str++;
    }
    
    if (*str == '\0') {
        *saveptr = str;
        return NULL;
    }
    
    token = str;
    
    /* 查找分隔符 */
    while (*str && !strchr(delim, *str)) {
        str++;
    }
    
    if (*str) {
        *str = '\0';
        *saveptr = str + 1;
    } else {
        *saveptr = str;
    }
    
    return token;
}

/* 整数转字符串 */
void itoa(int value, char* str, int base) {
    if (base < 2 || base > 36) {
        str[0] = '\0';
        return;
    }
    
    char* ptr = str, *ptr1 = str, tmp_char;
    int tmp_value;
    
    /* 处理负数 */
    if (value < 0 && base == 10) {
        *ptr++ = '-';
        value = -value;
    }
    
    do {
        tmp_value = value % base;
        value /= base;
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp_value];
    } while (value);
    
    *ptr-- = '\0';
    
    /* 反转字符串 */
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

/* 无符号整数转字符串 */
void utoa(uint32_t value, char* str, int base) {
    if (base < 2 || base > 36) {
        str[0] = '\0';
        return;
    }
    
    char* ptr = str, *ptr1 = str, tmp_char;
    uint32_t tmp_value;
    
    do {
        tmp_value = value % base;
        value /= base;
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp_value];
    } while (value);
    
    *ptr-- = '\0';
    
    /* 反转字符串 */
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

/* 字符串转换为整数 */
int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    
    /* 跳过空白字符 */
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }
    
    /* 处理符号 */
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    /* 转换数字 */
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return result * sign;
}

/* 字符串拼接辅助函数 */
void string_append(char* dest, const char* src) {
    while (*dest) dest++;
    while ((*dest++ = *src++));
}

/* 格式化字符串（简化版） */
int vsnprintf(char* str, size_t size, const char* format, va_list args) {
    // 简化实现，只处理 %s, %d, %u, %x
    char buffer[32];
    char* p = str;
    size_t remaining = size;
    
    while (*format && remaining > 1) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 's': {
                    const char* s = va_arg(args, const char*);
                    size_t len = strlen(s);
                    if (len >= remaining) len = remaining - 1;
                    memcpy(p, s, len);
                    p += len;
                    remaining -= len;
                    break;
                }
                case 'd': {
                    int n = va_arg(args, int);
                    itoa(n, buffer, 10);
                    size_t len = strlen(buffer);
                    if (len >= remaining) len = remaining - 1;
                    memcpy(p, buffer, len);
                    p += len;
                    remaining -= len;
                    break;
                }
                case 'u': {
                    uint32_t n = va_arg(args, uint32_t);
                    utoa(n, buffer, 10);
                    size_t len = strlen(buffer);
                    if (len >= remaining) len = remaining - 1;
                    memcpy(p, buffer, len);
                    p += len;
                    remaining -= len;
                    break;
                }
                case 'x': {
                    uint32_t n = va_arg(args, uint32_t);
                    utoa(n, buffer, 16);
                    size_t len = strlen(buffer);
                    if (len >= remaining) len = remaining - 1;
                    memcpy(p, buffer, len);
                    p += len;
                    remaining -= len;
                    break;
                }
                case '%': {
                    *p++ = '%';
                    remaining--;
                    break;
                }
                default:
                    /* 未知格式，跳过 */
                    break;
            }
            format++;
        } else {
            *p++ = *format++;
            remaining--;
        }
    }
    
    if (size > 0) {
        *p = '\0';
    }
    
    return p - str;
}

int snprintf(char* str, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(str, size, format, args);
    va_end(args);
    return result;
}