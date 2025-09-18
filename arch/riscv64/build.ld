OUTPUT_ARCH("riscv")
ENTRY(_start)

STACK_SIZE = 4096;          /* 单个核心栈大小（字节） */
RAM_BASE = 0x80000000;      /* RAM起始地址 */
RAM_SIZE = 2M;       /* RAM大小（2MB） */
HEAP_SIZE = 2M;        /* 堆大小（2MB） */

MEMORY 
{
    RAM (rwx) : ORIGIN = RAM_BASE, LENGTH = RAM_SIZE
}

SECTIONS
{
    . = ORIGIN(RAM);
    .text : ALIGN(4) 
    {
        . = ALIGN(4096);
        _text_start = .;
         *(.text .text.*)
        . = ALIGN(4096);
        _text_end = .;
    } > RAM

    .rodata : ALIGN(8) 
    {
        . = ALIGN(4096);
       _rodata_start = .; 
       *(.rodata .rodata.* .srodata .srodata.*)
        . = ALIGN(4096);
       _rodata_end = .;
    } > RAM

    .data : ALIGN(8) 
    {
        _data_start = .;
        . = ALIGN(4096);
        *(.data .data.* .sdata .sdata.*)
        . = ALIGN(4096);
        _data_end = .; 
    } > RAM 

    .bss : ALIGN(8) 
    {
        _bss_start = .;
        *(.bss .bss.* .sbss )
        *(COMMON)
        . = ALIGN(4096);
        _bss_end = .;
    } > RAM 

    . = ALIGN(4096);
    
    _stack_end = RAM_BASE + RAM_SIZE;       /* 栈底（高地址） */
    _stack_start = _stack_end - STACK_SIZE;

    _heap_end = _stack_start;
    _heap_start = _heap_end - HEAP_SIZE;

    PROVIDE_HIDDEN(__text_start = _text_start);
    PROVIDE_HIDDEN(__text_end = _text_end);

    PROVIDE_HIDDEN(__rodata_start = _rodata_start);
    PROVIDE_HIDDEN(__rodata_end = _rodata_end);

    PROVIDE_HIDDEN(__data_start = _data_start);
    PROVIDE_HIDDEN(__data_end = _data_end);

    PROVIDE_HIDDEN(__bss_start = _bss_start);
    PROVIDE_HIDDEN(__bss_end = _bss_end);

    PROVIDE_HIDDEN(__stack_start = _stack_start);
    PROVIDE_HIDDEN(__stack_end = _stack_end);

    PROVIDE_HIDDEN(__heap_start = _heap_start);
    PROVIDE_HIDDEN(__heap_end = _heap_end);
}
