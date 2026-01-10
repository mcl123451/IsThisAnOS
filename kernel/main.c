/* IsThisAnOS */
#include <kernel/graphics.h>
#include <kernel/font.h>
#include <kernel/string.h>
#include <kernel/io.h>

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
static graphics_context_t gfx_ctx;
static uint8_t graphics_enabled = 0;

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
            for (int i = 0; i < 24 * 80; i++) {
                video[i] = video[i + 80];
            }
            for (int i = 24 * 80; i < 25 * 80; i++) {
                video[i] = 0x0F00;
            }
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

void graphics_demo() {
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
    
    // 10. 显示状态
    graphics_draw_string(&gfx_ctx, 100, 500, 
                        "Status: Graphics demo running", COLOR_GREEN);
    
    serial_puts("Graphics demo completed\n");
}

/* 内核主函数 */
void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    // 初始化串口
    serial_init();
    serial_puts("\n=== IsThisAnOS Starting ===\n");
    
    // 文本模式显示
    vga_puts("IsThisAnOS\n");
    vga_puts("==========\n\n");
    
    // 检查Multiboot2魔数
    char buf[32];
    serial_puts("Multiboot magic: 0x");
    utoa(magic, buf, 16);
    serial_puts(buf);
    serial_puts("\n");
    
    vga_puts("Magic: 0x");
    vga_puts(buf);
    vga_puts("\n");
    
    if (magic != 0x36d76289) {
        vga_puts("ERROR: Invalid Multiboot2 magic!\n");
        serial_puts("ERROR: Invalid Multiboot2 magic!\n");
        return;
    }
    
    vga_puts("Valid Multiboot2 kernel\n");
    serial_puts("Valid Multiboot2 kernel\n");
    
    // 尝试获取framebuffer信息
    vga_puts("\nInitializing graphics...\n");
    serial_puts("\nParsing Multiboot2 info for framebuffer...\n");
    
    if (parse_multiboot2_info(mb_info_addr)) {
        graphics_enabled = 1;
        vga_puts("Graphics initialized successfully!\n");
        serial_puts("Graphics initialized successfully!\n");
        
        // 运行图形演示
        graphics_demo();
        
        vga_puts("\nGraphics demo running.\n");
        vga_puts("Check display for output.\n");
    } else {
        vga_puts("Graphics initialization failed.\n");
        vga_puts("Running in text mode only.\n");
        serial_puts("Graphics initialization failed.\n");
        
        // 在文本模式显示更多信息
        vga_puts("\nAvailable memory: 640KB (simulated)\n");
        vga_puts("Kernel version: 0.1\n");
        vga_puts("Status: Running in text mode\n");
    }
    
    vga_puts("\nSystem ready.\n");
    vga_puts("Press Ctrl+Alt+Delete to reboot\n");
    
    // 主循环
    serial_puts("\nEntering main loop\n");
    
    while (1) {
        // 在文本模式下闪烁光标
        static unsigned int counter = 0;
        static int cursor_visible = 1;
        
        if (counter++ % 1000000 == 0) {
            cursor_visible = !cursor_visible;
            volatile unsigned short* video = (volatile unsigned short*)0xB8000;
            if (cursor_visible) {
                video[24 * 80 + 79] = 0x0F00 | '_';
            } else {
                video[24 * 80 + 79] = 0x0F00 | ' ';
            }
        }
        
        asm volatile("hlt");
    }
}