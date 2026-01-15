; 中断服务例程汇编部分
section .text

; 全局声明，供C代码调用
global idt_flush
global isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
global isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
global isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
global isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
global irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7
global irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15
global isr_common_stub, irq_common_stub

; 外部C函数声明
extern isr_handler
extern irq_handler

; 宏：定义无错误代码的异常处理程序
%macro ISR_NOERRCODE 1
isr%1:
    cli
    push byte 0          ; 压入伪错误代码
    push byte %1         ; 压入中断号
    jmp isr_common_stub
%endmacro

; 宏：定义有错误代码的异常处理程序
%macro ISR_ERRCODE 1
isr%1:
    cli
    push byte %1         ; 压入中断号
    jmp isr_common_stub
%endmacro

; 宏：定义IRQ处理程序
%macro IRQ 2
irq%1:
    cli
    push byte 0
    push byte %2
    jmp irq_common_stub
%endmacro

; 定义异常处理程序
ISR_NOERRCODE 0   ; 除法错误
ISR_NOERRCODE 1   ; 调试异常
ISR_NOERRCODE 2   ; NMI
ISR_NOERRCODE 3   ; 断点
ISR_NOERRCODE 4   ; 溢出
ISR_NOERRCODE 5   ; 越界
ISR_NOERRCODE 6   ; 无效操作码
ISR_NOERRCODE 7   ; 设备不可用
ISR_ERRCODE   8   ; 双重错误
ISR_NOERRCODE 9   ; 协处理器段超限
ISR_ERRCODE   10  ; 无效TSS
ISR_ERRCODE   11  ; 段不存在
ISR_ERRCODE   12  ; 栈段错误
ISR_ERRCODE   13  ; 通用保护错误
ISR_ERRCODE   14  ; 页错误
ISR_NOERRCODE 15  ; 保留
ISR_NOERRCODE 16  ; 浮点错误
ISR_ERRCODE   17  ; 对齐检查
ISR_NOERRCODE 18  ; 机器检查
ISR_NOERRCODE 19  ; SIMD浮点异常
; ... 继续定义20-31，这些通常是保留的
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; 定义IRQ处理程序
IRQ  0, 32   ; 定时器
IRQ  1, 33   ; 键盘
IRQ  2, 34   ; 级联
IRQ  3, 35   ; COM2
IRQ  4, 36   ; COM1
IRQ  5, 37   ; LPT2
IRQ  6, 38   ; 软盘
IRQ  7, 39   ; LPT1
IRQ  8, 40   ; CMOS时钟
IRQ  9, 41   ; 自由
IRQ 10, 42   ; 自由
IRQ 11, 43   ; 自由
IRQ 12, 44   ; PS/2鼠标
IRQ 13, 45   ; 协处理器
IRQ 14, 46   ; 主IDE
IRQ 15, 47   ; 从IDE

; 通用ISR存根
isr_common_stub:
    ; 保存所有通用寄存器
    pusha
    
    ; 保存段寄存器
    push ds
    push es
    push fs
    push gs
    
    ; 加载内核数据段选择子
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; 调用C ISR处理函数（传递栈指针作为参数）
    push esp
    call isr_handler
    add esp, 4
    
    ; 恢复段寄存器
    pop gs
    pop fs
    pop es
    pop ds
    
    ; 恢复通用寄存器
    popa
    
    ; 清理栈上的错误代码和中断号
    add esp, 8
    
    ; 中断返回
    iret

; 通用IRQ存根
irq_common_stub:
    ; 保存所有通用寄存器
    pusha
    
    ; 保存段寄存器
    push ds
    push es
    push fs
    push gs
    
    ; 加载内核数据段选择子
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; 调用C IRQ处理函数（传递栈指针作为参数）
    push esp
    call irq_handler
    add esp, 4
    
    ; 恢复段寄存器
    pop gs
    pop fs
    pop es
    pop ds
    
    ; 恢复通用寄存器
    popa
    
    ; 清理栈上的错误代码和中断号
    add esp, 8
    
    ; 中断返回
    iret

; IDT加载函数
idt_flush:
    mov eax, [esp + 4]  ; 获取参数（idt_ptr地址）
    lidt [eax]          ; 加载IDT
    ret

; 其他辅助函数
global enable_interrupts
enable_interrupts:
    sti
    ret

global disable_interrupts
disable_interrupts:
    cli
    ret