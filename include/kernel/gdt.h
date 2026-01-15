#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// GDT条目结构
struct gdt_entry {
    uint16_t limit_low;     // 段界限 0-15 位
    uint16_t base_low;      // 段基址 0-15 位
    uint8_t  base_middle;   // 段基址 16-23 位
    uint8_t  access;        // 访问字节（类型、特权级等）
    uint8_t  granularity;   // 段界限高 4 位 + 标志位
    uint8_t  base_high;     // 段基址 24-31 位
} __attribute__((packed));

// GDTR 寄存器结构
struct gdt_ptr {
    uint16_t limit;         // GDT 表界限
    uint32_t base;          // GDT 表基址
} __attribute__((packed));

// 段选择子定义
#define KERNEL_CODE_SEG 0x08  // 内核代码段选择子
#define KERNEL_DATA_SEG 0x10  // 内核数据段选择子
#define USER_CODE_SEG   0x18  // 用户代码段选择子 (DPL=3)
#define USER_DATA_SEG   0x20  // 用户数据段选择子 (DPL=3)
#define TSS_SEG         0x28  // TSS 段选择子

// 访问字节位定义
#define GDT_ACCESS_PRESENT     (1 << 7)      // 段存在位
#define GDT_ACCESS_PRIV_KERNEL (0 << 5)      // 特权级 0 (内核)
#define GDT_ACCESS_PRIV_USER   (3 << 5)      // 特权级 3 (用户)
#define GDT_ACCESS_SYSTEM      (0 << 4)      // 系统段
#define GDT_ACCESS_CODE_DATA   (1 << 4)      // 代码/数据段
#define GDT_ACCESS_EXECUTABLE  (1 << 3)      // 可执行（代码段）
#define GDT_ACCESS_READ_WRITE  (1 << 1)      // 可读/可写
#define GDT_ACCESS_ACCESSED    (1 << 0)      // 已访问位

// 粒度标志位定义
#define GDT_FLAG_32BIT         (1 << 6)      // 32位保护模式
#define GDT_FLAG_4K_GRANULARITY (1 << 7)     // 4KB粒度

// 函数声明
void gdt_init(void);
void gdt_load(void);
void gdt_set_entry(int index, uint32_t base, uint32_t limit, 
                   uint8_t access, uint8_t flags);

#endif