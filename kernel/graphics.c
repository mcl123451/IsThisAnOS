#include <kernel/graphics.h>
#include <kernel/font.h>

#define NULL ((void*)0)
static graphics_context_t* current_ctx = NULL;

/* 内部像素绘制函数 */
void put_pixel(uint32_t x, uint32_t y, uint32_t yor) {
    if (!current_ctx || x >= current_ctx->width || y >= current_ctx->height) 
        return;
    
    current_ctx->framebuffer[y * (current_ctx->pitch / 4) + x] = yor;
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

void graphics_draw_pixel(graphics_context_t* ctx, int x, int y, uint32_t color) {
    if (x < 0 || x >= ctx->width || y < 0 || y >= ctx->height) {
        return;
    }
    
    // 计算像素位置
    uint8_t* pixel_ptr = (uint8_t*)ctx->framebuffer + y * ctx->pitch + x * (ctx->bpp / 8);
    
    // 根据bpp写入像素值
    if (ctx->bpp == 32) {
        *((uint32_t*)pixel_ptr) = color;
    } else if (ctx->bpp == 24) {
        // 假设颜色格式为0xRRGGBB，按字节写入
        pixel_ptr[0] = (color >> 16) & 0xFF; // 红
        pixel_ptr[1] = (color >> 8) & 0xFF;  // 绿
        pixel_ptr[2] = color & 0xFF;         // 蓝
    } else if (ctx->bpp == 16) {
        *((uint16_t*)pixel_ptr) = (uint16_t)color;
    } else if (ctx->bpp == 8) {
        *pixel_ptr = (uint8_t)color;
    }
}

/* 清屏 */
void graphics_clear_screen(graphics_context_t* ctx, uint32_t yor) {
    if (!ctx) return;
    current_ctx = ctx;
    
    for (uint32_t y = 0; y < ctx->height; y++) {
        for (uint32_t x = 0; x < ctx->width; x++) {
            put_pixel(x, y, yor);
        }
    }
}

/* 绘制矩形 */
void graphics_draw_rect(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                        uint32_t width, uint32_t height, uint32_t yor) {
    if (!ctx) return;
    current_ctx = ctx;
    
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            put_pixel(x + j, y + i, yor);
        }
    }
}

/* 绘制边框矩形 */
void graphics_draw_rect_outline(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                               uint32_t width, uint32_t height, uint32_t yor) {
    if (!ctx) return;
    current_ctx = ctx;
    
    // 上边框
    for (uint32_t i = 0; i < width; i++) {
        put_pixel(x + i, y, yor);
    }
    // 下边框
    for (uint32_t i = 0; i < width; i++) {
        put_pixel(x + i, y + height - 1, yor);
    }
    // 左边框
    for (uint32_t i = 0; i < height; i++) {
        put_pixel(x, y + i, yor);
    }
    // 右边框
    for (uint32_t i = 0; i < height; i++) {
        put_pixel(x + width - 1, y + i, yor);
    }
}

/* 绘制水平线 */
void graphics_draw_hline(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                        uint32_t length, uint32_t yor) {
    if (!ctx) return;
    current_ctx = ctx;
    
    for (uint32_t i = 0; i < length; i++) {
        put_pixel(x + i, y, yor);
    }
}

/* 绘制垂直线 */
void graphics_draw_vline(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                        uint32_t length, uint32_t yor) {
    if (!ctx) return;
    current_ctx = ctx;
    
    for (uint32_t i = 0; i < length; i++) {
        put_pixel(x, y + i, yor);
    }
}

/* 绘制字符 */
void graphics_draw_char(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                       char c, uint32_t yor) {
    if (!ctx) return;
    current_ctx = ctx;
    draw_char(x, y, c, yor);
}

/* 绘制字符串 */
void graphics_draw_string(graphics_context_t* ctx, uint32_t x, uint32_t y, 
                         const char* str, uint32_t yor) {
    if (!ctx) return;
    current_ctx = ctx;
    draw_string(x, y, str, yor);
}

uint32_t graphics_get_pixel(graphics_context_t* ctx, int x, int y) {
    if (x < 0 || x >= ctx->width || y < 0 || y >= ctx->height) {
        return 0; // 或者根据你的错误处理方式返回
    }
    
    // 计算像素位置
    uint8_t* pixel_ptr = (uint8_t*)ctx->framebuffer + y * ctx->pitch + x * (ctx->bpp / 8);
    
    // 根据bpp读取像素值
    if (ctx->bpp == 32) {
        return *((uint32_t*)pixel_ptr);
    } else if (ctx->bpp == 24) {
        // 注意：24位像素可能是按字节对齐的，这里假设为RGB顺序
        return (pixel_ptr[0] << 16) | (pixel_ptr[1] << 8) | pixel_ptr[2];
    } else if (ctx->bpp == 16) {
        return *((uint16_t*)pixel_ptr);
    } else if (ctx->bpp == 8) {
        return *pixel_ptr;
    }
    
    return 0;
}