#include "kernel/idt.h"
#include "kernel/gdt.h"
#include "kernel/pic.h"
#include "kernel/io.h"
#include <stddef.h>
#include <stdbool.h>

// IDT 表（256个中断门）
static struct idt_entry idt[256];
static struct idt_ptr idtp;

// 中断处理函数指针数组
static interrupt_handler_t interrupt_handlers[256];

// 初始化 IDT
void idt_init(void) {
    // 1. 设置 IDT 指针
    idtp.limit = sizeof(struct idt_entry) * 256 - 1;
    idtp.base = (uint32_t)&idt;
    
    // 2. 初始化所有中断处理程序指针
    for (int i = 0; i < 256; i++) {
        interrupt_handlers[i] = NULL;
    }
    
    // 3. 设置异常处理程序（0-31）
    idt_set_gate(0, (uint32_t)isr0, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(1, (uint32_t)isr1, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(2, (uint32_t)isr2, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(3, (uint32_t)isr3, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(4, (uint32_t)isr4, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(5, (uint32_t)isr5, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(6, (uint32_t)isr6, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(7, (uint32_t)isr7, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(8, (uint32_t)isr8, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(9, (uint32_t)isr9, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(10, (uint32_t)isr10, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(11, (uint32_t)isr11, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(12, (uint32_t)isr12, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(13, (uint32_t)isr13, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(14, (uint32_t)isr14, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(15, (uint32_t)isr15, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(16, (uint32_t)isr16, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(17, (uint32_t)isr17, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(18, (uint32_t)isr18, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(19, (uint32_t)isr19, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(20, (uint32_t)isr20, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(21, (uint32_t)isr21, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(22, (uint32_t)isr22, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(23, (uint32_t)isr23, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(24, (uint32_t)isr24, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(25, (uint32_t)isr25, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(26, (uint32_t)isr26, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(27, (uint32_t)isr27, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(28, (uint32_t)isr28, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(29, (uint32_t)isr29, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(30, (uint32_t)isr30, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(31, (uint32_t)isr31, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    
    // 4. 设置 IRQ 处理程序（32-47）
    idt_set_gate(32, (uint32_t)irq0, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(33, (uint32_t)irq1, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(34, (uint32_t)irq2, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(35, (uint32_t)irq3, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(36, (uint32_t)irq4, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(37, (uint32_t)irq5, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(38, (uint32_t)irq6, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(39, (uint32_t)irq7, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(40, (uint32_t)irq8, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(41, (uint32_t)irq9, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(42, (uint32_t)irq10, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(43, (uint32_t)irq11, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(44, (uint32_t)irq12, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(45, (uint32_t)irq13, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(46, (uint32_t)irq14, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    idt_set_gate(47, (uint32_t)irq15, KERNEL_CODE_SEG, IDT_FLAG_32BIT_INT);
    
    // 5. 初始化PIC（8259A）
    pic_init();
    
    // 6. 加载 IDT
    asm volatile("lidt %0" : : "m"(idtp));
}

// 设置单个IDT门
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags | IDT_FLAG_PRESENT;
}

// 注册中断处理程序
void register_interrupt_handler(uint8_t n, interrupt_handler_t handler) {
    interrupt_handlers[n] = handler;
}

// 注册IRQ处理程序并启用该IRQ
void register_irq_handler(uint8_t irq, interrupt_handler_t handler) {
    uint8_t int_no = IRQ_BASE + irq;
    
    // 注册处理程序
    interrupt_handlers[int_no] = handler;
    
    // 启用该IRQ
    pic_enable_irq(irq);
}

// 默认中断处理函数
static void default_handler(struct registers *regs) {
    // 可以在这里显示错误信息
    // 例如：打印 "Unhandled interrupt X"
    
    // 如果是IRQ，需要发送EOI
    if (regs->int_no >= 32 && regs->int_no < 48) {
        // 发送EOI给PIC
        outb(0x20, 0x20);  // 主PIC
        if (regs->int_no >= 40) {
            outb(0xA0, 0x20);  // 从PIC
        }
    }
}

// ISR处理函数
void isr_handler(struct registers *regs) {
    uint8_t int_no = regs->int_no;
    
    if (interrupt_handlers[int_no]) {
        interrupt_handlers[int_no](regs);
    } else {
        // 未处理的中断
        // 可以打印错误信息
    }
}

// IRQ处理函数
void irq_handler(struct registers *regs) {
    uint8_t int_no = regs->int_no;
    
    // 调用处理程序
    if (interrupt_handlers[int_no]) {
        interrupt_handlers[int_no](regs);
    }
    
    // 发送EOI
    pic_send_eoi(int_no - IRQ_BASE);
}