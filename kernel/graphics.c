#include <kernel/graphics.h>
#include <kernel/font.h>

static graphics_context_t* current_ctx = NULL;

/* 内部像素绘制函数 */
void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!current_ctx || x >= current_ctx->width || y >= current_ctx->height) 
        return;
    
    current_ctx->framebuffer[y * (current_ctx->pitch / 4) + x] = color;
}

/* 初始化图形上下文 */
void graphics_init(graphics_context_t* ctx, uint32_t* framebuffer, 
                   uint32_t width, uint32_t height, uint32_t pitch, uint8_t bpp) {
    ctx->framebuffer = framebuffer;
    ctx->width = width;
    ctx->height = height;
    ctx->pitch = pitch;
    ctx->bpp = bpp;
    current_ctx = ctx;
}

/* 绘制像素 - 外部接口 */
void graphics_draw_pixel(graphics_context_t* ctx, uint32_t x, uint32_t y, uint32_t color) {
    if (!ctx) return;
    current_ctx = ctx;
    put_pixel(x, y, color);
}

/* 清屏 */
void graphics_clear_screen(graphics_context_t* ctx, uint32_t color) {
    if (!ctx) return;
    current_ctx = ctx;
    
    for (uint32_t y = 0; y < ctx->height; y++) {
        for (uint32_t x = 0; x < ctx->width; x++) {
            put_pixel(x, y, color);
        }
    }
}

/* 绘制矩形 */
void graphics_draw_rect(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                        uint32_t width, uint32_t height, uint32_t color) {
    if (!ctx) return;
    current_ctx = ctx;
    
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

/* 绘制边框矩形 */
void graphics_draw_rect_outline(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                               uint32_t width, uint32_t height, uint32_t color) {
    if (!ctx) return;
    current_ctx = ctx;
    
    // 上边框
    for (uint32_t i = 0; i < width; i++) {
        put_pixel(x + i, y, color);
    }
    // 下边框
    for (uint32_t i = 0; i < width; i++) {
        put_pixel(x + i, y + height - 1, color);
    }
    // 左边框
    for (uint32_t i = 0; i < height; i++) {
        put_pixel(x, y + i, color);
    }
    // 右边框
    for (uint32_t i = 0; i < height; i++) {
        put_pixel(x + width - 1, y + i, color);
    }
}

/* 绘制水平线 */
void graphics_draw_hline(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                        uint32_t length, uint32_t color) {
    if (!ctx) return;
    current_ctx = ctx;
    
    for (uint32_t i = 0; i < length; i++) {
        put_pixel(x + i, y, color);
    }
}

/* 绘制垂直线 */
void graphics_draw_vline(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                        uint32_t length, uint32_t color) {
    if (!ctx) return;
    current_ctx = ctx;
    
    for (uint32_t i = 0; i < length; i++) {
        put_pixel(x, y + i, color);
    }
}

/* 绘制字符 */
void graphics_draw_char(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                       char c, uint32_t color) {
    if (!ctx) return;
    current_ctx = ctx;
    draw_char(x, y, c, color);
}

/* 绘制字符串 */
void graphics_draw_string(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                         const char* str, uint32_t color) {
    if (!ctx) return;
    current_ctx = ctx;
    draw_string(x, y, str, color);
}