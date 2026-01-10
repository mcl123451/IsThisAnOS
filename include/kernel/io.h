#ifndef KERNEL_IO_H
#define KERNEL_IO_H

#include <kernel/types.h>

/* 端口 I/O */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

/* 串口函数 */
void serial_init();
void serial_putc(char c);
void serial_puts(const char* str);

#endif /* KERNEL_IO_H */