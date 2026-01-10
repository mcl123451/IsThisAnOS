#ifndef KERNEL_FONT_H
#define KERNEL_FONT_H

#include <kernel/types.h>
#include <kernel/graphics.h>  /* 添加图形头文件 */

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

/* ASCII 0-127 字体数据 */
extern const uint8_t font_data[128][16];

/* 字体渲染函数 */
void font_init(graphics_context_t* ctx);  /* 添加初始化函数 */
void draw_char(uint32_t x, uint32_t y, char c, uint32_t color);
void draw_string(uint32_t x, uint32_t y, const char* str, uint32_t color);

#endif /* KERNEL_FONT_H */