#ifndef KERNEL_STRING_H
#define KERNEL_STRING_H

#include <kernel/types.h>

/* 可变参数支持 */
typedef __builtin_va_list va_list;
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)

/* 字符串长度 */
size_t strlen(const char* str);

/* 内存操作 */
void* memset(void* ptr, uint8_t value, size_t num);
void* memcpy(void* dest, const void* src, size_t num);
void* memmove(void* dest, const void* src, size_t num);
int memcmp(const void* ptr1, const void* ptr2, size_t num);

/* 字符串操作 */
int strcmp(const char* str1, const char* str2);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
size_t strlcpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
char* strchr(const char* str, int c);
char* strrchr(const char* str, int c);
char* strtok_r(char* str, const char* delim, char** saveptr);

/* 字符串与数字转换 */
void itoa(int value, char* str, int base);
void utoa(uint32_t value, char* str, int base);
int atoi(const char* str);

/* 字符串辅助函数 */
void string_append(char* dest, const char* src);

/* 格式化输出 */
int vsnprintf(char* str, size_t size, const char* format, va_list args);
int snprintf(char* str, size_t size, const char* format, ...);

#endif /* KERNEL_STRING_H */