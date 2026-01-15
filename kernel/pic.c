#include "kernel/pic.h"
#include "kernel/io.h"

// 初始化PIC（8259A可编程中断控制器）
void pic_init(void) {
    // 开始初始化序列
    
    // 1. ICW1: 开始初始化
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);  // 边缘触发，级联，需要ICW4
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    
    // 2. ICW2: 重新映射中断向量
    outb(PIC1_DATA, 0x20);    // 主PIC IRQ0-7 -> 中断号32-39
    io_wait();
    outb(PIC2_DATA, 0x28);    // 从PIC IRQ8-15 -> 中断号40-47
    io_wait();
    
    // 3. ICW3: 级联连接
    outb(PIC1_DATA, 0x04);    // 主PIC IRQ2连接从PIC
    io_wait();
    outb(PIC2_DATA, 0x02);    // 从PIC连接到主PIC的IRQ2
    io_wait();
    
    // 4. ICW4: 8086模式
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();
    
    // 5. 屏蔽所有中断（稍后按需启用）
    outb(PIC1_DATA, 0xFF);    // 屏蔽主PIC所有中断
    outb(PIC2_DATA, 0xFF);    // 屏蔽从PIC所有中断
}

// 启用指定IRQ
void pic_enable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    
    // 读取当前屏蔽字，清除对应位（0=启用）
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

// 禁用指定IRQ
void pic_disable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    
    // 读取当前屏蔽字，设置对应位（1=禁用）
    value = inb(port) | (1 << irq);
    outb(port, value);
}

// 发送EOI (End Of Interrupt)
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_CMD, PIC_EOI);  // 从PIC
    }
    outb(PIC1_CMD, PIC_EOI);      // 主PIC
}

// 等待一小段时间（用于端口操作）
void io_wait(void) {
    outb(0x80, 0);  // 写入一个空端口
}