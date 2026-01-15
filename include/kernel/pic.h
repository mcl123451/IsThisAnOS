#ifndef PIC_H
#define PIC_H

#include <stdint.h>

// PIC 端口定义
#define PIC1_CMD     0x20    // 主PIC命令端口
#define PIC1_DATA    0x21    // 主PIC数据端口
#define PIC2_CMD     0xA0    // 从PIC命令端口
#define PIC2_DATA    0xA1    // 从PIC数据端口

// PIC EOI 命令
#define PIC_EOI      0x20    // End Of Interrupt

// 初始化控制字
#define ICW1_INIT    0x10    // 初始化命令
#define ICW1_ICW4    0x01    // 需要ICW4
#define ICW4_8086    0x01    // 8086模式

// 函数声明
void pic_init(void);
void pic_enable_irq(uint8_t irq);
void pic_disable_irq(uint8_t irq);
void pic_send_eoi(uint8_t irq);
void io_wait(void);

#endif