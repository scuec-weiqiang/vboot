#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "types.h"
extern uintptr_t boot_text_start;
extern uintptr_t boot_text_end;
extern uintptr_t boot_text_size;
extern uintptr_t boot_rodata_start;
extern uintptr_t boot_rodata_end;
extern uintptr_t boot_rodata_size;
extern uintptr_t boot_data_start;
extern uintptr_t boot_data_end;
extern uintptr_t boot_data_size;
extern uintptr_t boot_bss_start;
extern uintptr_t boot_bss_end;
extern uintptr_t boot_bss_size;
extern uintptr_t boot_stack_start;
extern uintptr_t boot_stack_end;
extern uintptr_t boot_stack_size;
extern uintptr_t boot_heap_start;
extern uintptr_t boot_heap_end;
extern uintptr_t boot_heap_size;
extern uintptr_t core_num;

extern void symbols_init();
extern void zero_bss();

#endif