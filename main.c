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
#include "boot_malloc.h"
#include "vm.h"
#include "virt_disk.h"
#include "vfs.h"
#include "elf.h"
#include "mm.h"
#include "riscv.h"
#include "symbols.h"

void init()
{
    zero_bss();
    symbols_init();

    satp_w(0);
    medeleg_w(medeleg_r()|0xffffffff);// 将所有异常委托给S模式处理
    mideleg_w(mideleg_r()|(1<<1)|(1<<9)); // 将s模式软件中断，外部中断委托给s模式
    
    // 将整个用户空间（39位）设置保护
    pmpaddr0_w(0x3fffffffffffff);
    pmpcfg0_w(0xf);
    
    // 使能S模式外部中断，定时器中断和软件中断
    virt_disk_init();
    fs_init();
    malloc_init();

    while(1)
    {

    }

    // struct file *f = vfs_open(path, 0);
    // char *elf = malloc(f->f_inode->i_size);
    // ssize_t ret = vfs_read(f, elf, f->f_inode->i_size);

    // struct elf_info *elf_info = elf_parse(elf);

    // pgtbl_t *user_pgd = page_alloc(1);
    // memset(user_pgd, 0, sizeof(pgtbl_t));

    // u8 *user_stack = malloc(PROC_STACK_SIZE);
    // memset(user_stack, 0, PROC_STACK_SIZE);

    // u8 *kernel_stack = malloc(PROC_STACK_SIZE);
    // memset(kernel_stack, 0, PROC_STACK_SIZE);

    // for (int i = 0; i < elf_info->phnum; i++)
    // {
    //     if (elf_info->segs[i].type == PT_LOAD)
    //     {
    //         printk("phdr %d: vaddr:%x, memsz:%x, filesz:%x, offset:%x, flags:%x\n", i, elf_info->segs[i].vaddr, elf_info->segs[i].memsz, elf_info->segs[i].filesz, elf_info->segs[i].offset, elf_info->segs[i].flags);
    //         u8 *user_space = malloc(elf_info->segs[i].memsz); // 程序加载到内存里需要的空间
    //         memset(user_space, 0, elf_info->segs[i].memsz);
    //         memcpy(user_space, elf + elf_info->segs[i].offset, elf_info->segs[i].filesz);
    //         map_pages(user_pgd, elf_info->segs[i].vaddr, (uintptr_t)user_space, (elf_info->segs[i].memsz + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE, PTE_R | PTE_X | PTE_U);
    //     }
    // }
    // map_pages(user_pgd, PROC_USER_STACK_BOTTOM, (uintptr_t)user_stack, PROC_STACK_SIZE, PTE_W | PTE_R | PTE_U);
    // page_table_init(user_pgd);

    // sscratch_w(sp_r());
    // satp_w(MAKE_SATP(p->pgd));
    // sstatus_w(sstatus_r() & ~(1 << 8));
    // sepc_w((uintptr_t)(p->context.sepc));
    // asm volatile("mv sp,%0" ::"r"(p->user_sp));
    // asm volatile("sret");
}

// proc_init();
// proc_t* init_proc = proc_create("/user.elf");
// proc_run(init_proc);

