/* IsThisAnOS */
#include <kernel/graphics.h>
#include <kernel/font.h>
#include <kernel/string.h>
#include <kernel/io.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/pic.h>
#include <kernel/mouse.h>

/* Multiboot2 信息结构 */
typedef struct {
    uint32_t total_size;
    uint32_t reserved;
} multiboot2_info_header_t;

/* 标签头 */
typedef struct {
    uint32_t type;
    uint32_t size;
} multiboot_tag_header_t;

/* 帧缓冲信息标签 */
typedef struct {
    multiboot_tag_header_t header;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t reserved;
} multiboot_tag_framebuffer_t;

/* 图形上下文 */
graphics_context_t gfx_ctx;
uint8_t graphics_enabled = 0;

/* VGA 文本输出 */
void vga_puts(const char* str) {
    volatile unsigned short* video = (volatile unsigned short*)0xB8000;
    static int x = 0, y = 0;
    while (*str) {
        if (*str == '\n') {
            x = 0;
            y++;
        } else {
            video[y * 80 + x] = (0x0F << 8) | *str;
            x++;
            if (x >= 80) {
                x = 0;
                y++;
            }
        }
        if (y >= 25) {
            // 简单滚屏
            for (int i = 0; i < 24 * 80; i++)
                video[i] = video[i + 80];
            for (int i = 24 * 80; i < 25 * 80; i++)
                video[i] = 0x0F00;
            y = 24;
        }
        str++;
    }
}

/* 解析Multiboot2信息，查找framebuffer */
int parse_multiboot2_info(uint32_t mb_info_addr) {
    char buf[32];
    
    // 检查是否为有效的Multiboot2信息结构
    if (mb_info_addr == 0) {
        serial_puts("Invalid Multiboot2 info address (0)\n");
        return 0;
    }
    
    multiboot2_info_header_t* header = (multiboot2_info_header_t*)mb_info_addr;
    serial_puts("Multiboot2 info total size: ");
    utoa(header->total_size, buf, 10);
    serial_puts(buf);
    serial_puts(" bytes\n");
    
    // 遍历所有标签
    uint32_t offset = 8; // 跳过总大小和保留字段
    int framebuffer_found = 0;
    
    while (offset < header->total_size) {
        multiboot_tag_header_t* tag = (multiboot_tag_header_t*)(mb_info_addr + offset);
        
        serial_puts("Found tag type: ");
        utoa(tag->type, buf, 10);
        serial_puts(buf);
        serial_puts(", size: ");
        utoa(tag->size, buf, 10);
        serial_puts(buf);
        serial_puts("\n");
        
        if (tag->type == 0) {
            // 结束标签
            break;
        }
        
        if (tag->type == 8) { // Framebuffer信息标签
            multiboot_tag_framebuffer_t* fb_tag = (multiboot_tag_framebuffer_t*)tag;
            
            serial_puts("FOUND FRAMEBUFFER INFO!\n");
            serial_puts("  Address: 0x");
            utoa((uint32_t)fb_tag->framebuffer_addr, buf, 16);
            serial_puts(buf);
            serial_puts("\n");
            
            serial_puts("  Width: ");
            utoa(fb_tag->framebuffer_width, buf, 10);
            serial_puts(buf);
            serial_puts("\n");
            
            serial_puts("  Height: ");
            utoa(fb_tag->framebuffer_height, buf, 10);
            serial_puts(buf);
            serial_puts("\n");
            
            serial_puts("  Pitch: ");
            utoa(fb_tag->framebuffer_pitch, buf, 10);
            serial_puts(buf);
            serial_puts("\n");
            
            serial_puts("  BPP: ");
            utoa(fb_tag->framebuffer_bpp, buf, 10);
            serial_puts(buf);
            serial_puts("\n");
            
            // 初始化图形上下文
            if (fb_tag->framebuffer_addr != 0) {
                graphics_init(&gfx_ctx, 
                            (uint32_t*)fb_tag->framebuffer_addr,
                            fb_tag->framebuffer_width,
                            fb_tag->framebuffer_height,
                            fb_tag->framebuffer_pitch,
                            fb_tag->framebuffer_bpp);
                framebuffer_found = 1;
                break;
            }
        }
        // 移动到下一个标签（对齐到8字节）
        offset += (tag->size + 7) & ~7;
    }
    return framebuffer_found;
}


// 定时器中断处理程序
void timer_handler(struct registers *regs) {
    static int ticks = 0;
    ticks++;
    
    // 发送EOI
    outb(0x20, 0x20);
}

// 键盘中断处理程序（IRQ1）
void keyboard_handler(struct registers *regs) {
    uint8_t scancode = inb(0x60);
    
    // 检查是否是按键按下（扫描码最高位为0表示按下）
    if (scancode < 0x80) {
        // 简单的键盘映射表
        const char *keymap = "??1234567890-=??qwertyuiop[]\n?asdfghjkl;'`?\\zxcvbnm,./?*? ?";
        
        if (scancode < sizeof(keymap) && keymap[scancode] != '?') {
            char c = keymap[scancode];
            serial_puts("Key pressed: ");
            char str[2] = {c, '\0'};
            serial_puts(str);
            serial_puts("\n");
        }
    }
}

// 页错误处理程序（异常14）
void page_fault_handler(struct registers *regs) {
    uint32_t faulting_address;
    
    // 读取CR2寄存器获取错误地址
    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));
    
    // 在屏幕上显示错误信息
    serial_puts("\n!!! PAGE FAULT !!!\n");
    
    // 显示错误地址（简化版）
    char addr_str[12];
    itoa((int)faulting_address, addr_str, 16);
    serial_puts("Faulting address: ");
    serial_puts(addr_str);
    serial_puts("\n");
    
    // 显示错误代码
    char err_str[12];
    itoa((int)regs->err_code, err_str, 10);
    serial_puts("Error code: ");
    serial_puts(err_str);
    serial_puts("\n");
    
    // 死循环
    serial_puts("System halted.\n");
    asm volatile("cli\n""hlt");
}

// 双重错误处理程序（异常8）
void double_fault_handler(struct registers *regs) {
    serial_puts("\n!!! DOUBLE FAULT !!!\n");
    serial_puts("System halted.\n");
    asm volatile("cli\n""hlt");
}

// 通用保护错误处理程序（异常13）
void general_protection_fault_handler(struct registers *regs) {
    serial_puts("\n!!! GENERAL PROTECTION FAULT !!!\n");
    
    char err_str[12];
    itoa((int)regs->err_code, err_str, 10);
    serial_puts("Error code: ");
    serial_puts(err_str);
    serial_puts("\n");
    
    serial_puts("System halted.\n");
    asm volatile("cli\n""hlt");
}

// 除零错误处理程序（异常0）
void divide_by_zero_handler(struct registers *regs) {
    serial_puts("\n!!! DIVIDE BY ZERO !!!\n");
    serial_puts("System halted.\n");
    asm volatile("cli\n""hlt");
}

/* 绘制桌面图形 */
void graphics_desktop() {
    if (!graphics_enabled) return;
    serial_puts("Starting graphics demo\n");
    // 1. 清屏为深蓝色
    graphics_clear_screen(&gfx_ctx, 0x000033);
    // 2. 显示标题
    graphics_draw_string(&gfx_ctx, gfx_ctx.width/2 - 150, 50, 
                        "IsThisAnOS Graphical Kernel", COLOR_WHITE);
    // 3. 显示分辨率信息
    char res_str[64];
    utoa(gfx_ctx.width, res_str, 10);
    strcat(res_str, " x ");
    char height_str[16];
    utoa(gfx_ctx.height, height_str, 10);
    strcat(res_str, height_str);
    strcat(res_str, " x ");
    char bpp_str[16];
    utoa(gfx_ctx.bpp, bpp_str, 10);
    strcat(res_str, bpp_str);
    graphics_draw_string(&gfx_ctx, gfx_ctx.width/2 - 100, 80, 
                        res_str, COLOR_CYAN);
    
    // 4. 绘制彩色矩形
    graphics_draw_rect(&gfx_ctx, 100, 150, 200, 100, COLOR_RED);
    graphics_draw_rect(&gfx_ctx, 350, 150, 200, 100, COLOR_GREEN);
    graphics_draw_rect(&gfx_ctx, 600, 150, 200, 100, COLOR_BLUE);
    
    // 5. 绘制边框矩形
    graphics_draw_rect_outline(&gfx_ctx, 95, 145, 210, 110, COLOR_WHITE);
    graphics_draw_rect_outline(&gfx_ctx, 345, 145, 210, 110, COLOR_WHITE);
    graphics_draw_rect_outline(&gfx_ctx, 595, 145, 210, 110, COLOR_WHITE);
    
    // 6. 在矩形上显示文字
    graphics_draw_string(&gfx_ctx, 180, 190, "RED", COLOR_WHITE);
    graphics_draw_string(&gfx_ctx, 430, 190, "GREEN", COLOR_WHITE);
    graphics_draw_string(&gfx_ctx, 680, 190, "BLUE", COLOR_WHITE);
    
    // 7. 绘制线条
    graphics_draw_hline(&gfx_ctx, 100, 300, 700, COLOR_YELLOW);
    graphics_draw_vline(&gfx_ctx, 450, 320, 150, COLOR_MAGENTA);
    
    // 8. 显示功能列表
    graphics_draw_string(&gfx_ctx, 100, 350, 
                        "- Framebuffer graphics", COLOR_LIGHT_GRAY);
    graphics_draw_string(&gfx_ctx, 100, 370, 
                        "- Bitmap font rendering", COLOR_LIGHT_GRAY);
    graphics_draw_string(&gfx_ctx, 100, 390, 
                        "- Basic shape drawing", COLOR_LIGHT_GRAY);
    graphics_draw_string(&gfx_ctx, 100, 410, 
                        "- Color support (32-bit)", COLOR_LIGHT_GRAY);
    
    // 9. 绘制彩虹条
    uint32_t rainbow[] = {
        0xFF0000,  // 红
        0xFF7F00,  // 橙
        0xFFFF00,  // 黄
        0x00FF00,  // 绿
        0x0000FF,  // 蓝
        0x4B0082,  // 靛
        0x9400D3   // 紫
    };

    int bar_width = 100;
    for (int i = 0; i < 7; i++) {
        graphics_draw_rect(&gfx_ctx, 100 + i * bar_width, 450, 
                          bar_width - 10, 30, rainbow[i]);
    }
    
    // 10. 绘制鼠标指针形状
    draw_mouse(mouse_get_x(), mouse_get_y());
    // 11. 显示按钮提示
    graphics_draw_string(&gfx_ctx, 100, 600, 
                        "Click on colored rectangles with mouse!", COLOR_YELLOW);
    
    // 12. 显示状态
    graphics_draw_string(&gfx_ctx, 100, 500, 
                        "Status: Graphics running", COLOR_GREEN);
    serial_puts("Graphics completed\n");
}


/* 检查鼠标点击事件 */
void check_mouse_click(void) {
    static uint32_t last_check_time = 0;
    static uint8_t last_button_state = 0;
    
    if (mouse_check_button_click(MOUSE_LEFT_BUTTON)) {
        int mouse_x = mouse_get_x();
        int mouse_y = mouse_get_y();
        
        char click_str[64];
        itoa(mouse_x, click_str, 10);
        strcat(click_str, ",");
        char y_str[16];
        itoa(mouse_y, y_str, 10);
        strcat(click_str, y_str);
        
        // 检查点击区域
        if (mouse_x >= 100 && mouse_x <= 300 && 
            mouse_y >= 150 && mouse_y <= 250) {
            graphics_draw_string(&gfx_ctx, 150, 270, "Clicked!", COLOR_YELLOW);
            serial_puts("Clicked on RED rectangle!\n");
        } else if (mouse_x >= 350 && mouse_x <= 550 && 
                   mouse_y >= 150 && mouse_y <= 250) {
            graphics_draw_string(&gfx_ctx, 400, 270, "Clicked!", COLOR_YELLOW);
            serial_puts("Clicked on GREEN rectangle!\n");
        } else if (mouse_x >= 600 && mouse_x <= 800 && 
                   mouse_y >= 150 && mouse_y <= 250) {
            graphics_draw_string(&gfx_ctx, 650, 270, "Clicked!", COLOR_YELLOW);
            serial_puts("Clicked on BLUE rectangle!\n");
        }
    }
}

/* 内核主函数 */
void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    // 初始化串口
    serial_init();
    serial_puts("\n=== IsThisAnOS Starting ===\n");
    
    // 检查Multiboot2魔数
    char buf[32];
    serial_puts("Multiboot magic: 0x");
    utoa(magic, buf, 16);
    serial_puts(buf);
    serial_puts("\n");
    
    if (magic != 0x36d76289) {
        vga_puts("ERROR: Invalid Multiboot2 magic!\n");
        serial_puts("ERROR: Invalid Multiboot2 magic!\n");
        return;
    }
    
    // 尝试获取framebuffer信息
    vga_puts("\nInitializing graphics...\n");
    serial_puts("\nParsing Multiboot2 info for framebuffer...\n");
    
    if (parse_multiboot2_info(mb_info_addr)) {
        graphics_enabled = 1;
        serial_puts("Graphics initialized successfully!\n");
    
        asm volatile("cli");
        gdt_init();
        idt_init();
        // 注册异常处理程序
        register_interrupt_handler(0, divide_by_zero_handler);      // 除零错误
        register_interrupt_handler(8, double_fault_handler);        // 双重错误
        register_interrupt_handler(13, general_protection_fault_handler); // 通用保护错误
        register_interrupt_handler(14, page_fault_handler);         // 页错误
        
        // 注册IRQ处理程序
        register_irq_handler(0, timer_handler);      // 定时器
        register_irq_handler(1, keyboard_handler);   // 键盘
        register_irq_handler(12, mouse_handler);     // 鼠标（PS/2）
        
        // 启用IRQ
        pic_enable_irq(0);   // 定时器
        pic_enable_irq(1);   // 键盘
        pic_enable_irq(2);   // 级联中断（必须启用）
        pic_enable_irq(12);  // 鼠标

        // 初始化定时器
        int divisor = 1193180 / 100;  // 100Hz
        outb(0x43, 0x36);
        outb(0x40, divisor & 0xFF);
        outb(0x40, (divisor >> 8) & 0xFF);
        
        // 初始化键盘
        outb(0x64, 0xAE);  // 启用键盘接口
        
        // 初始化鼠标
        mouse_init();

        asm volatile("sti");
        // 运行图形界面
        graphics_desktop();
        
        vga_puts("\nGraphics running.\n");
        vga_puts("Check display for output.\n");
    } else {
        vga_puts("Graphics initialization failed.\n");
        serial_puts("Graphics initialization failed.\n");
    }
    
    // 主循环
    serial_puts("\nEntering main loop\n");
    
    uint32_t frame_count = 0;
    uint32_t mouse_update_counter = 0;
    
    while (1) {
        asm volatile("hlt");
        
        frame_count++;

        mouse_update();
        
        check_mouse_click();
        
        // 每100帧强制重绘鼠标
        if (frame_count % 100 == 0) {
            mouse_force_redraw();
        }
        
        // 防止帧计数溢出
        if (frame_count >= 1000000) {
            frame_count = 0;
        }
    }
}