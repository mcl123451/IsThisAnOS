#include <kernel/io.h>

/* 串口端口 */
#define COM1 0x3F8

/* 初始化串口 */
void serial_init() {
    outb(COM1 + 1, 0x00);    // 禁用所有中断
    outb(COM1 + 3, 0x80);    // 启用 DLAB (设置波特率除数)
    outb(COM1 + 0, 0x03);    // 设置除数为 3 (lo byte) 38400 波特
    outb(COM1 + 1, 0x00);    //                  (hi byte)
    outb(COM1 + 3, 0x03);    // 8位, 无奇偶校验, 一个停止位
    outb(COM1 + 2, 0xC7);    // 启用 FIFO, 清除, 14字节阈值
    outb(COM1 + 4, 0x0B);    // IRQ启用, RTS/DSR设置
}

/* 检查串口是否空闲 */
int serial_is_transmit_empty() {
    return inb(COM1 + 5) & 0x20;
}

/* 通过串口发送字符 */
void serial_putc(char c) {
    while (serial_is_transmit_empty() == 0);
    outb(COM1, c);
}

/* 通过串口发送字符串 */
void serial_puts(const char* str) {
    while (*str) {
        serial_putc(*str++);
    }
}