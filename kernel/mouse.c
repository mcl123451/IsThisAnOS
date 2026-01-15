#include "kernel/mouse.h"
#include "kernel/graphics.h"
#include "kernel/io.h"
#include <stddef.h>
#include <stdint.h>

/* 外部图形上下文声明 */
extern graphics_context_t gfx_ctx;
extern uint8_t graphics_enabled;

/* 简化的鼠标光标形状 (16x16) */
static const uint8_t mouse_cursor_data[16 * 16] = {
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
    1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0
};

/* 全局鼠标状态 */
static mouse_state_t mouse_state = {
    .x = 400,
    .y = 300,
    .old_x = 400,
    .old_y = 300,
    .buttons = 0,
    .packet_byte = 0,
    .packet = {0, 0, 0, 0},
    .visible = 1,
    .dirty = 0
};

/* 点击检测相关状态 */
static struct {
    uint8_t current_buttons;    // 当前按钮状态
    uint8_t last_buttons;       // 上一次按钮状态
    uint8_t button_down[3];     // 按钮按下状态 [左, 右, 中]
    uint8_t button_up[3];       // 按钮释放状态
    uint32_t click_time[3];     // 按钮按下时间
    int click_count[3];         // 连击计数
    uint32_t last_click_time[3]; // 上一次点击时间
} click_state;

/* 缓冲区 */
static uint32_t back_buffer[16 * 16];

/* 队列系统 */
#define MOUSE_QUEUE_SIZE 16
static struct {
    int8_t dx;
    int8_t dy;
    uint8_t buttons;
    uint32_t timestamp;
} mouse_queue[MOUSE_QUEUE_SIZE];
static int queue_head = 0;
static int queue_tail = 0;
static int queue_count = 0;

/* 添加到队列 */
void enqueue_mouse_data(int8_t dx, int8_t dy, uint8_t buttons) {
    if (queue_count >= MOUSE_QUEUE_SIZE) {
        // 队列满，丢弃最旧的数据
        queue_head = (queue_head + 1) % MOUSE_QUEUE_SIZE;
        queue_count--;
    }
    
    int tail = (queue_head + queue_count) % MOUSE_QUEUE_SIZE;
    mouse_queue[tail].dx = dx;
    mouse_queue[tail].dy = dy;
    mouse_queue[tail].buttons = buttons;
    mouse_queue[tail].timestamp = 0; // 可以用时间戳
    
    queue_count++;
}

/* 从队列取出 */
int dequeue_mouse_data(int8_t *dx, int8_t *dy, uint8_t *buttons) {
    if (queue_count == 0) {
        return 0;
    }
    
    *dx = mouse_queue[queue_head].dx;
    *dy = mouse_queue[queue_head].dy;
    *buttons = mouse_queue[queue_head].buttons;
    
    queue_head = (queue_head + 1) % MOUSE_QUEUE_SIZE;
    queue_count--;
    
    return 1;
}

/* 保存背景 */
void save_background(int x, int y) {
    if (!graphics_enabled || x < 0 || y < 0 || 
        x + 16 > gfx_ctx.width || y + 16 > gfx_ctx.height) {
        return;
    }
    
    for (int py = 0; py < 16; py++) {
        for (int px = 0; px < 16; px++) {
            back_buffer[py * 16 + px] = graphics_get_pixel(&gfx_ctx, x + px, y + py);
        }
    }
}

/* 恢复背景 */
void restore_background(int x, int y) {
    if (!graphics_enabled || x < 0 || y < 0 || 
        x + 16 > gfx_ctx.width || y + 16 > gfx_ctx.height) {
        return;
    }
    
    for (int py = 0; py < 16; py++) {
        for (int px = 0; px < 16; px++) {
            graphics_draw_pixel(&gfx_ctx, x + px, y + py, back_buffer[py * 16 + px]);
        }
    }
}

/* 绘制鼠标指针 */
void draw_mouse(int x, int y) {
    if (!graphics_enabled || x < 0 || y < 0 || 
        x + 16 > gfx_ctx.width || y + 16 > gfx_ctx.height) {
        return;
    }
    
    // 根据按钮状态改变鼠标颜色
    uint32_t color = COLOR_WHITE;
    if (mouse_state.buttons & 0x01) {
        color = COLOR_RED;  // 左键按下时变为红色
    } else if (mouse_state.buttons & 0x02) {
        color = COLOR_BLUE;  // 右键按下时变为蓝色
    }
    
    for (int py = 0; py < 16; py++) {
        for (int px = 0; px < 16; px++) {
            if (mouse_cursor_data[py * 16 + px]) {
                graphics_draw_pixel(&gfx_ctx, x + px, y + py, color);
            }
        }
    }
}

/* 更新点击检测状态 */
void update_click_detection(uint8_t new_buttons) {
    uint8_t old_buttons = click_state.current_buttons;
    click_state.current_buttons = new_buttons;
    
    // 检测按钮按下事件
    for (int i = 0; i < 3; i++) {
        uint8_t mask = 1 << i;
        
        // 检测按下事件
        if ((new_buttons & mask) && !(old_buttons & mask)) {
            click_state.button_down[i] = 1;
            click_state.click_time[i] = 0; // 重置按下时间
            // 这里可以触发按下事件
        }
        
        // 检测释放事件
        if (!(new_buttons & mask) && (old_buttons & mask)) {
            click_state.button_up[i] = 1;
            
            // 检查是否为有效点击（按下时间小于500ms）
            if (click_state.click_time[i] < 50) { // 假设每10ms更新一次
                click_state.click_count[i]++;
                click_state.last_click_time[i] = click_state.click_time[i];
                // 这里可以触发点击事件
            }
        }
    }
    
    // 更新按下时间
    for (int i = 0; i < 3; i++) {
        if (click_state.current_buttons & (1 << i)) {
            if (click_state.click_time[i] < 100) { // 最大1秒
                click_state.click_time[i]++;
            }
        }
    }
}

/* 鼠标初始化 */
void mouse_init(void) {
    // 启用鼠标
    outb(0x64, 0xA8);
    
    // 启用中断
    outb(0x64, 0x20);
    uint8_t config = inb(0x60);
    config |= 0x02;  // 启用鼠标中断
    config |= 0x01;  // 启用键盘中断
    outb(0x64, 0x60);
    outb(0x60, config);
    
    // 设置默认设置
    outb(0x64, 0xD4);
    outb(0x60, 0xF6);
    (void)inb(0x60);  // 读取确认
    
    // 启用数据报告
    outb(0x64, 0xD4);
    outb(0x60, 0xF4);
    (void)inb(0x60);  // 读取确认
    
    // 设置采样率（提高响应性）
    outb(0x64, 0xD4);
    outb(0x60, 0xF3);  // 设置采样率
    outb(0x60, 0xC8);  // 200采样/秒
    
    outb(0x64, 0xD4);
    outb(0x60, 0xF3);
    outb(0x60, 0x64);  // 100采样/秒
    
    outb(0x64, 0xD4);
    outb(0x60, 0xF3);
    outb(0x60, 0x50);  // 80采样/秒
    
    // 清空缓冲区
    while ((inb(0x64) & 0x01) == 1) {
        inb(0x60);
    }
    
    // 初始化点击检测状态
    click_state.current_buttons = 0;
    click_state.last_buttons = 0;
    for (int i = 0; i < 3; i++) {
        click_state.button_down[i] = 0;
        click_state.button_up[i] = 0;
        click_state.click_time[i] = 0;
        click_state.click_count[i] = 0;
        click_state.last_click_time[i] = 0;
    }
    
    // 初始化队列
    queue_head = 0;
    queue_tail = 0;
    queue_count = 0;
    
    // 保存初始背景并绘制鼠标
    if (graphics_enabled) {
        save_background(mouse_state.x, mouse_state.y);
        draw_mouse(mouse_state.x, mouse_state.y);
    }
}

/* 鼠标中断处理程序 - 优化点击检测 */
void mouse_handler(struct registers *regs) {
    uint8_t status = inb(0x64);
    
    if (!(status & 0x20)) {
        outb(0xA0, 0x20);
        outb(0x20, 0x20);
        return;
    }
    
    uint8_t data = inb(0x60);
    
    // 处理数据包
    if (mouse_state.packet_byte == 0 && (data & 0x08)) {
        mouse_state.packet[0] = data;
        mouse_state.packet_byte = 1;
    } else if (mouse_state.packet_byte == 1) {
        mouse_state.packet[1] = data;
        mouse_state.packet_byte = 2;
    } else if (mouse_state.packet_byte == 2) {
        mouse_state.packet[2] = data;
        mouse_state.packet_byte = 0;
        
        // 解析数据包
        uint8_t flags = mouse_state.packet[0];
        int8_t dx = (int8_t)mouse_state.packet[1];
        int8_t dy = (int8_t)mouse_state.packet[2];
        uint8_t buttons = flags & 0x07;
        
        // 立即更新点击检测
        update_click_detection(buttons);
        
        // 标记按钮状态变化
        if (buttons != mouse_state.buttons) {
            mouse_state.buttons = buttons;
            mouse_state.dirty = 1;  // 标记需要重绘
        }
        
        // 如果有移动或按钮变化，添加到队列
        if (dx != 0 || dy != 0 || (buttons != click_state.last_buttons)) {
            enqueue_mouse_data(dx, dy, buttons);
            click_state.last_buttons = buttons;
        }
    }
    
    outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

/* 更新鼠标显示和状态 */
void mouse_update(void) {
    if (!mouse_state.visible || !graphics_enabled) {
        return;
    }
    
    int8_t dx = 0, dy = 0;
    uint8_t buttons = mouse_state.buttons;
    int has_movement = 0;
    
    // 处理队列中的所有数据
    while (dequeue_mouse_data(&dx, &dy, &buttons)) {
        has_movement = 1;
        
        // 更新鼠标位置
        int new_x = mouse_state.x + dx;
        int new_y = mouse_state.y - dy;  // Y轴反向
        
        // 边界检查static 
        if (new_x < 0) new_x = 0;
        if (new_y < 0) new_y = 0;
        if (new_x > gfx_ctx.width - 16) new_x = gfx_ctx.width - 16;
        if (new_y > gfx_ctx.height - 16) new_y = gfx_ctx.height - 16;
        
        // 如果位置改变，更新鼠标显示
        if (new_x != mouse_state.x || new_y != mouse_state.y) {
            restore_background(mouse_state.x, mouse_state.y);
            save_background(new_x, new_y);
            
            mouse_state.x = new_x;
            mouse_state.y = new_y;
        }
        
        // 更新按钮状态
        if (buttons != mouse_state.buttons) {
            mouse_state.buttons = buttons;
            mouse_state.dirty = 1;
        }
    }
    
    // 如果需要重绘鼠标（按钮状态变化）
    if (mouse_state.dirty) {
        restore_background(mouse_state.x, mouse_state.y);
        save_background(mouse_state.x, mouse_state.y);
        draw_mouse(mouse_state.x, mouse_state.y);
        mouse_state.dirty = 0;
    }
    // 如果有移动，重绘鼠标
    else if (has_movement) {
        draw_mouse(mouse_state.x, mouse_state.y);
    }
}

/* 检查按钮按下事件 */
int mouse_check_button_press(uint8_t button_mask) {
    for (int i = 0; i < 3; i++) {
        if ((button_mask & (1 << i)) && click_state.button_down[i]) {
            click_state.button_down[i] = 0;  // 清除标志
            return 1;
        }
    }
    return 0;
}

/* 检查按钮释放事件 */
int mouse_check_button_release(uint8_t button_mask) {
    for (int i = 0; i < 3; i++) {
        if ((button_mask & (1 << i)) && click_state.button_up[i]) {
            click_state.button_up[i] = 0;  // 清除标志
            return 1;
        }
    }
    return 0;
}

/* 检查按钮点击事件（按下并释放） */
int mouse_check_button_click(uint8_t button_mask) {
    for (int i = 0; i < 3; i++) {
        if ((button_mask & (1 << i)) && click_state.click_count[i] > 0) {
            click_state.click_count[i]--;
            return 1;
        }
    }
    return 0;
}

/* 获取鼠标状态 */
mouse_state_t* mouse_get_state(void) {
    return &mouse_state;
}

/* 获取鼠标X坐标 */
int mouse_get_x(void) {
    return mouse_state.x;
}

/* 获取鼠标Y坐标 */
int mouse_get_y(void) {
    return mouse_state.y;
}

/* 获取鼠标按钮状态 */
uint8_t mouse_get_buttons(void) {
    return mouse_state.buttons;
}

/* 检查是否按下左键 */
int mouse_is_left_pressed(void) {
    return (mouse_state.buttons & 0x01) != 0;
}

/* 检查是否按下右键 */
int mouse_is_right_pressed(void) {
    return (mouse_state.buttons & 0x02) != 0;
}

/* 检查是否按下中键 */
int mouse_is_middle_pressed(void) {
    return (mouse_state.buttons & 0x04) != 0;
}

/* 设置鼠标可见性 */
void mouse_set_visible(uint8_t visible) {
    if (mouse_state.visible == visible) return;
    
    mouse_state.visible = visible;
    
    if (!visible) {
        restore_background(mouse_state.x, mouse_state.y);
    } else {
        draw_mouse(mouse_state.x, mouse_state.y);
    }
}

/* 强制重绘鼠标 */
void mouse_force_redraw(void) {
    if (!mouse_state.visible || !graphics_enabled) return;
    
    draw_mouse(mouse_state.x, mouse_state.y);
}

/* 检查队列是否为空 */
int mouse_queue_empty(void) {
    return queue_count == 0;
}

/* 获取队列中的数据数量 */
int mouse_queue_size(void) {
    return queue_count;
}