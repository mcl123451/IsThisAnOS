/* IsThisAnOS 链接脚本 - 简化版本 */
OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    /* 内核从1MB地址开始 */
    . = 1M;

    /* Multiboot2 头部必须在最前面 */
    .multiboot ALIGN(8) : {
        KEEP(*(.multiboot))
    }

    /* 代码段 */
    .text ALIGN(4K) : {
        *(.text)
    }

    /* 只读数据 */
    .rodata ALIGN(4K) : {
        *(.rodata)
    }

    /* 数据段 */
    .data ALIGN(4K) : {
        *(.data)
    }

    /* BSS 段 */
    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
    }

    /* 内核结束标记 */
    _end = .;

    /* 丢弃不需要的节 */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.*)
    }
}