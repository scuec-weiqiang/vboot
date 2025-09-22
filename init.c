/**
 * @FilePath: /vboot/init.c
 * @Description:
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-05-07 19:18:08
 * @LastEditTime: 2025-09-21 16:26:50
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
#include "string.h"

#define ALIGN_UP(x, align) (((x) + (align)-1) & ~((align)-1))
u64 read_csr_mc(void){ u64 x; asm volatile("csrr %0, mcause":"=r"(x)); return x; }
u64 read_csr_mt(void){ u64 x; asm volatile("csrr %0, mtval":"=r"(x)); return x; }
u64 read_csr_mepc(void){ u64 x; asm volatile("csrr %0, mepc":"=r"(x)); return x; }


void trap()
{
    printk("mcause=%xu mepc=%xu mtval=%u\n",
        read_csr_mc(), read_csr_mepc(), read_csr_mt());
    while (1)
    {
        /* code */
    }
    
}

void jump_to_kernel()
{
    struct file *kernel_img = open("/ZZZ-OS.img",0);
    char *elf = (char*)malloc((size_t)kernel_img->f_inode->i_size);
    read(kernel_img,elf,kernel_img->f_inode->i_size);
    struct elf_info *elf_info = elf_parse(elf);

    u8 *kernel_space = (u8*)(uintptr_t)boot_stack_end; // 程序加载到内存里需要的空间
    u64 offset = 0;
    for (int i = 0; i < elf_info->phnum; i++)
    {
        if (elf_info->segs[i].type == PT_LOAD)
        {
            
            memset(kernel_space+offset, 0, elf_info->segs[i].memsz);
            memcpy(kernel_space+offset, elf + elf_info->segs[i].offset, elf_info->segs[i].filesz);
            offset += elf_info->segs[i].memsz;
        }
    }
    
    mstatus_w(sstatus_r() & ~(3<<11));
    mstatus_w(sstatus_r() | (1<<11));
    mepc_w((uintptr_t)elf_info->entry);
    asm volatile("mret");
}

/**
* @brief 初始化函数
*
* 该函数执行一系列的初始化操作，包括：
* 1. 调用 zero_bss 函数来初始化 BSS 段。
* 2. 调用 symbols_init 函数进行符号初始化。
* 3. 设置机器陷阱向量寄存器 mtvec 为 trap 函数的地址。
* 4. 将 satp 寄存器清零。
* 5. 将所有异常委托给 S 模式处理，通过 medeleg_w 函数实现。
* 6. 将软件中断、外部中断委托给 S 模式处理，通过 mideleg_w 函数实现。
* 7. 调用 vm_init 函数进行虚拟机初始化。
* 8. 使能 S 模式外部中断、定时器中断和软件中断。
* 9. 调用 M_TO_S 函数将当前模式切换到 S 模式。
*/
void init()
{
    zero_bss();
    symbols_init();
    mtvec_w((uintptr_t)trap);
    satp_w(0);
    medeleg_w(medeleg_r()|0xffffffff);// 将所有异常委托给S模式处理
    mideleg_w(mideleg_r()|(1<<1)|(1<<5)|(1<<9)); // 将软件中断，外部中断委托给s模式

    // 将整个用户空间（39位）设置保护
    pmpaddr0_w(0x3fffffffffffff);
    pmpcfg0_w(0xf);

    stvec_w((uintptr_t)trap);
    malloc_init();
    virt_disk_init();
    fs_init();
    early_page_table_init();

    jump_to_kernel();
}


// proc_init();
// proc_t* init_proc = proc_create("/user.elf");
// proc_run(init_proc);

