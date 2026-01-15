[BITS 32]

; Multiboot2 头部 - 正确请求 framebuffer
section .multiboot
align 8

multiboot_header_start:
    dd 0xE85250D6                ; Multiboot2 魔数
    dd 0                         ; 架构：i386
    dd multiboot_header_end - multiboot_header_start  ; 头部长度
    dd 0x100000000 - (0xE85250D6 + 0 + (multiboot_header_end - multiboot_header_start)) ; 校验和

    ; 信息请求标签 - 请求 framebuffer 信息
    align 8
    dw 1                         ; 类型：信息请求
    dw 0                         ; 标志
    dd 8                         ; 大小：8字节（仅标签头）
    ; 没有额外请求，这样GRUB会提供所有可用信息

    ; framebuffer 标签 - 请求特定的图形模式
    align 8
    dw 5                         ; 类型：framebuffer
    dw 0                         ; 标志
    dd 20                        ; 大小：20字节
    dd 1024                      ; 宽度
    dd 768                       ; 高度
    dd 32                        ; 深度

    ; 结束标签
    align 8
    dw 0                         ; 类型：结束
    dw 0                         ; 标志
    dd 8                         ; 大小
multiboot_header_end:

section .bss
align 16
stack_bottom:
    resb 16384                   ; 16KB栈
stack_top:

section .text

; 入口点
global _start
extern kernel_main

_start:
    ; 设置栈指针
    mov esp, stack_top
    
    ; 保存Multiboot2信息
    ; EAX = 魔数 (应该是 0x36d76289)
    ; EBX = Multiboot2信息结构地址
    push ebx
    push eax
    
    ; 清除方向标志
    cld
    
    ; 调用内核主函数
    call kernel_main
    
    ; 如果内核返回，进入无限循环
    cli
.hang:
    hlt
    jmp .hang