#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include "kernel/graphics.h"
#include "kernel/idt.h"

/* 鼠标状态结构 */
typedef struct {
    int x, y;
    int old_x, old_y;
    uint8_t buttons;
    int packet_byte;
    uint8_t packet[4];
    uint8_t visible;
    uint8_t dirty;
} mouse_state_t;

/* 按钮掩码 */
#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_RIGHT_BUTTON  0x02
#define MOUSE_MIDDLE_BUTTON 0x04

/* 函数声明 */
void mouse_init(void);
void mouse_handler(struct registers *regs);
void mouse_update(void);
void draw_mouse(int x, int y);
/* 状态获取 */
mouse_state_t* mouse_get_state(void);
int mouse_get_x(void);
int mouse_get_y(void);
uint8_t mouse_get_buttons(void);
int mouse_is_left_pressed(void);
int mouse_is_right_pressed(void);
int mouse_is_middle_pressed(void);

/* 点击检测 */
int mouse_check_button_press(uint8_t button_mask);
int mouse_check_button_release(uint8_t button_mask);
int mouse_check_button_click(uint8_t button_mask);

/* 显示控制 */
void mouse_set_visible(uint8_t visible);
void mouse_force_redraw(void);

/* 队列状态 */
int mouse_queue_empty(void);
int mouse_queue_size(void);

#endif // MOUSE_H