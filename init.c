/**
 * @FilePath: /vboot/main.c
 * @Description:
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-05-07 19:18:08
 * @LastEditTime: 2025-09-18 00:05:05
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
 */

#include "printk.h"
#include "malloc.h"
#include "vm.h"
#include "virt_disk.h"
#include "vfs.h"
#include "elf.h"
#include "mm.h"
#include "riscv.h"
#include "symbols.h"

void trap()
{
    while (1)
    {
        /* code */
    }
    
}

void s_mode()
{
    malloc_init();
    virt_disk_init();
    fs_init();
    early_page_table_init();

    struct file *kernel_img = open("/user.img",0);
    char *elf = (char*)malloc((size_t)kernel_img->f_inode->i_size);
    read(kernel_img,elf,kernel_img->f_inode->i_size);
    struct elf_info *elf_info = elf_parse(elf);

    u8 *user_space = 0x80200000; // 程序加载到内存里需要的空间
    u64 offset = 0;
    for (int i = 0; i < elf_info->phnum; i++)
    {
        if (elf_info->segs[i].type == PT_LOAD)
        {
            memset(user_space+offset, 0, elf_info->segs[i].memsz);
            memcpy(user_space+offset, elf + elf_info->segs[i].offset, elf_info->segs[i].filesz);
            offset += elf_info->segs[i].memsz;
        }
    }
    sstatus_w(sstatus_r() & ~(3<<11));
    sstatus_w(sstatus_r() | (1<<11));
    sepc_w((uintptr_t)elf_info->entry);
    asm volatile("sret");
}

void init()
{
    zero_bss();
    symbols_init();
    mtvec_w((uintptr_t)trap);
    satp_w(0);
    medeleg_w(medeleg_r()|0xffffffff);// 将所有异常委托给S模式处理
    mideleg_w(mideleg_r()|(1<<1)|(1<<5)|(1<<9)); // 将软件中断，外部中断委托给s模式
    vm_init();
    // 使能S模式外部中断，定时器中断和软件中断

    M_TO_S(s_mode);
}


// proc_init();
// proc_t* init_proc = proc_create("/user.elf");
// proc_run(init_proc);

