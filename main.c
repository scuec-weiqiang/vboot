/**
 * @FilePath: /vboot/main.c
 * @Description:  
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-05-07 19:18:08
 * @LastEditTime: 2025-09-18 00:05:05
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
*/
#include "printf.h"
#include "page_alloc.h"

#include "vm.h"
#include "virt_disk.h"
#include "time.h"

#include "vfs.h"
#include "elf.h"
#include "proc.h"

u8 is_init = 0;

void init_kernel()
{  
    hart_id_t hart_id = 0;
    if(hart_id == 0) // hart0 初始化全局资源
    {
        virt_disk_init(); 
        vfs_init();
        
        
        // proc_init();
        // proc_t* init_proc = proc_create("/user.elf");
        // proc_run(init_proc);

        is_init = 1;

    }

    printf("hart_id:%d\n", hart_id);
    while (is_init == 0){}

    s_global_interrupt_enable(); 
   
   
   
 }