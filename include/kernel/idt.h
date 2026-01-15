#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// 寄存器状态结构体（由汇编压栈创建）
struct registers {
    uint32_t gs, fs, es, ds;                     // 段寄存器
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // 通用寄存器 (pusha顺序)
    uint32_t int_no, err_code;                   // 中断号和错误代码
    uint32_t eip, cs, eflags, user_esp, ss;      // 由处理器自动压入（特权级可能变化）
} __attribute__((packed));

// 中断门描述符结构
struct idt_entry {
    uint16_t base_low;      // 处理程序地址低16位
    uint16_t selector;      // 代码段选择子
    uint8_t  zero;          // 必须为0
    uint8_t  flags;         // 门类型和属性
    uint16_t base_high;     // 处理程序地址高16位
} __attribute__((packed));

// IDTR 寄存器结构
struct idt_ptr {
    uint16_t limit;         // IDT表界限
    uint32_t base;          // IDT表基址
} __attribute__((packed));

// 中断门标志位定义
#define IDT_FLAG_PRESENT     (1 << 7)
#define IDT_FLAG_DPL_0       (0 << 5)    // 特权级0（内核）
#define IDT_FLAG_DPL_3       (3 << 5)    // 特权级3（用户）
#define IDT_FLAG_STORAGE     (0 << 4)    // 存储段描述符
#define IDT_FLAG_32BIT_INT   (0xE)       // 32位中断门
#define IDT_FLAG_32BIT_TRAP  (0xF)       // 32位陷阱门

// 异常中断号定义
#define IDT_EXCEPTION_COUNT 32

// IRQ 中断号定义
#define IRQ_BASE 32
#define IRQ_COUNT 16

// 常用中断号
#define IRQ_TIMER   (IRQ_BASE + 0)
#define IRQ_KEYBOARD (IRQ_BASE + 1)

// 函数声明
void idt_init(void);
void idt_load(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

// 中断处理函数类型
typedef void (*interrupt_handler_t)(struct registers *);

// 注册中断处理程序
void register_interrupt_handler(uint8_t n, interrupt_handler_t handler);
void register_irq_handler(uint8_t irq, interrupt_handler_t handler);

// C中断处理函数
void isr_handler(struct registers *regs);
void irq_handler(struct registers *regs);

// 外部汇编函数声明
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
// ... 声明所有32个异常处理函数
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

#endif