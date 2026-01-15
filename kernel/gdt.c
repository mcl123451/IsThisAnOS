#include "kernel/gdt.h"
#include <stddef.h>

// GDT 表
static struct gdt_entry gdt[6];
static struct gdt_ptr gp;

// 外部汇编函数声明
extern void gdt_flush(uint32_t);

// 设置单个GDT条目
void gdt_set_entry(int index, uint32_t base, uint32_t limit, 
                   uint8_t access, uint8_t flags) {
    struct gdt_entry *entry = &gdt[index];
    
    // 设置段基址
    entry->base_low = (base & 0xFFFF);
    entry->base_middle = (base >> 16) & 0xFF;
    entry->base_high = (base >> 24) & 0xFF;
    
    // 设置段界限
    entry->limit_low = (limit & 0xFFFF);
    
    // 组合粒度标志和界限高4位
    entry->granularity = ((limit >> 16) & 0x0F) | (flags & 0xF0);
    
    // 设置访问字节
    entry->access = access;
}

// 初始化 GDT
void gdt_init(void) {
    // 设置 GDT 指针
    gp.limit = (sizeof(struct gdt_entry) * 6) - 1;
    gp.base = (uint32_t)&gdt;
    
    // 空描述符（索引 0，必须为0）
    gdt_set_entry(0, 0, 0, 0, 0);
    
    // 内核代码段
    gdt_set_entry(1, 0, 0xFFFFFFFF,
                  GDT_ACCESS_PRESENT | GDT_ACCESS_CODE_DATA | 
                  GDT_ACCESS_EXECUTABLE | GDT_ACCESS_READ_WRITE,
                  GDT_FLAG_32BIT | GDT_FLAG_4K_GRANULARITY);
    
    // 内核数据段
    gdt_set_entry(2, 0, 0xFFFFFFFF,
                  GDT_ACCESS_PRESENT | GDT_ACCESS_CODE_DATA | 
                  GDT_ACCESS_READ_WRITE,
                  GDT_FLAG_32BIT | GDT_FLAG_4K_GRANULARITY);
    
    // 用户代码段 (DPL = 3)
    gdt_set_entry(3, 0, 0xFFFFFFFF,
                  GDT_ACCESS_PRESENT | GDT_ACCESS_CODE_DATA | 
                  GDT_ACCESS_EXECUTABLE | GDT_ACCESS_READ_WRITE | 
                  GDT_ACCESS_PRIV_USER,
                  GDT_FLAG_32BIT | GDT_FLAG_4K_GRANULARITY);
    
    // 用户数据段 (DPL = 3)
    gdt_set_entry(4, 0, 0xFFFFFFFF,
                  GDT_ACCESS_PRESENT | GDT_ACCESS_CODE_DATA | 
                  GDT_ACCESS_READ_WRITE | GDT_ACCESS_PRIV_USER,
                  GDT_FLAG_32BIT | GDT_FLAG_4K_GRANULARITY);
    
    // 加载新的GDT
    gdt_load();
}

// 加载 GDT 并刷新段寄存器
void gdt_load(void) {
    // 加载 GDTR 寄存器
    asm volatile("lgdt %0" : : "m"(gp));
    
    // 重新加载段选择子
    asm volatile(
        "movw $0x10, %%ax\n"   // 内核数据段选择子
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        "movw %%ax, %%ss\n"
        "ljmp $0x08, $1f\n"    // 远跳转刷新CS
        "1:\n"
        : : : "eax"
    );
    serial_puts("GDT loaded successfully\n");
}