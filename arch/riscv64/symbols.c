/**
 * @FilePath: /vboot/arch/riscv64/symbols.c
 * @Description:  
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-09-17 13:05:59
 * @LastEditTime: 2025-09-21 14:57:14
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
*/
#include "types.h"

extern char __text_start[], __text_end[];
extern char __rodata_start[], __rodata_end[];
extern char __data_start[], __data_end[];
extern char __bss_start[], __bss_end[];
extern char __stack_start[], __stack_end[];
extern char __heap_start[], __heap_end[];


uintptr_t boot_text_start;
uintptr_t boot_text_end;
uintptr_t boot_text_size;
uintptr_t boot_rodata_start;
uintptr_t boot_rodata_end;
uintptr_t boot_rodata_size;
uintptr_t boot_data_start;
uintptr_t boot_data_end;
uintptr_t boot_data_size;
uintptr_t boot_bss_start;
uintptr_t boot_bss_end;
uintptr_t boot_bss_size;
uintptr_t boot_stack_start;
uintptr_t boot_stack_end;
uintptr_t boot_stack_size;
uintptr_t boot_heap_start;
uintptr_t boot_heap_end;
uintptr_t boot_heap_size;


void symbols_init()
{
    boot_text_start = (uintptr_t)&__text_start;
    boot_text_end = (uintptr_t)&__text_end;
    boot_text_size = (boot_text_end - boot_text_start);
    boot_rodata_start = (uintptr_t)&__rodata_start;
    boot_rodata_end = (uintptr_t)&__rodata_end;
    boot_rodata_size = (boot_rodata_end - boot_rodata_start);
    boot_data_start = (uintptr_t)&__data_start;
    boot_data_end = (uintptr_t)&__data_end;
    boot_data_size = (boot_data_end - boot_data_start);
    boot_bss_start = (uintptr_t)&__bss_start;
    boot_bss_end = (uintptr_t)&__bss_end;
    boot_bss_size = (boot_bss_end - boot_bss_start);
    boot_stack_start = (uintptr_t)&__stack_start;
    boot_stack_end = (uintptr_t)&__stack_end;
    boot_stack_size = (boot_stack_end - boot_stack_start);
    boot_heap_start = (uintptr_t)&__heap_start;
    boot_heap_end = (uintptr_t)&__heap_end;
    boot_heap_size = (boot_heap_end - boot_heap_start);
}

/**
 * @brief 将BSS段中的所有数据清零
 *
 * 遍历BSS段的起始地址到结束地址之间的所有字节，并将它们置为零。
 *
 * BSS段通常用于存储未初始化的全局变量和静态变量，它们在程序启动时不会自动初始化为零。
 * 本函数通过手动遍历并清零这些变量，确保它们在程序启动时是干净的。
 */
void zero_bss() 
{
    for (char *p = (char*)boot_bss_start; p < (char*)boot_bss_end; p++) 
    {
        *p = 0;
    }
}