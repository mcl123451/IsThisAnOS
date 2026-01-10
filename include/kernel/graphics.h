#ifndef KERNEL_GRAPHICS_H
#define KERNEL_GRAPHICS_H

#include <kernel/types.h>

/* 颜色定义 */
#define COLOR_BLACK         0x000000
#define COLOR_BLUE          0x0000FF
#define COLOR_GREEN         0x00FF00
#define COLOR_CYAN          0x00FFFF
#define COLOR_RED           0xFF0000
#define COLOR_MAGENTA       0xFF00FF
#define COLOR_YELLOW        0xFFFF00
#define COLOR_WHITE         0xFFFFFF
#define COLOR_GRAY          0x808080
#define COLOR_DARK_GRAY     0x404040
#define COLOR_LIGHT_GRAY    0xC0C0C0

/* 图形上下文 */
typedef struct {
    uint32_t* framebuffer;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;
} graphics_context_t;

/* 函数声明 */
void put_pixel(uint32_t x, uint32_t y, uint32_t color);  /* 添加这一行 */
void graphics_init(graphics_context_t* ctx, uint32_t* framebuffer, 
                   uint32_t width, uint32_t height, uint32_t pitch, uint8_t bpp);
void graphics_draw_pixel(graphics_context_t* ctx, uint32_t x, uint32_t y, uint32_t color);
void graphics_clear_screen(graphics_context_t* ctx, uint32_t color);
void graphics_draw_rect(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                        uint32_t width, uint32_t height, uint32_t color);
void graphics_draw_rect_outline(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                               uint32_t width, uint32_t height, uint32_t color);
void graphics_draw_hline(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                        uint32_t length, uint32_t color);
void graphics_draw_vline(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                        uint32_t length, uint32_t color);
void graphics_draw_char(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                       char c, uint32_t color);
void graphics_draw_string(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                         const char* str, uint32_t color);

#endif /* KERNEL_GRAPHICS_H */