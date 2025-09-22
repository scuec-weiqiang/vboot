/**
 * @FilePath: /vboot/vm.c
 * @Description:  
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-05-08 22:00:50
 * @LastEditTime: 2025-09-21 15:57:49
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
*/
#include "printk.h"
#include "virtio.h"
#include "uart.h"
#include "malloc.h"
#include "riscv.h"
#include "mm.h"
#include "string.h"
#include "symbols.h"
#include "plic.h"
#include "clint.h"
#include "vm.h"
#include "check.h"

pgtbl_t* early_pgd = NULL;//kernel_page_global_directory 内核页全局目录

/**
 * @brief 从父页表中获取子页表
 *
 * 根据虚拟页号(vpn_level)和虚拟地址(va)从父页表(parent_pgd)中获取对应的子页表。
 * 如果子页表不存在且参数create为true，则创建子页表。
 *
 * @param parent_pgd 父页表的地址
 * @param vpn_level 虚拟页号
 * @param va 虚拟地址
 * @param create 是否创建子页表，true表示创建，false表示不创建
 *
 * @return 指向子页表的指针，如果不存在且不创建则返回NULL
 */
pgtbl_t* get_child_pgtbl(pgtbl_t *parent_pgd, u64 vpn_level, u64 va, bool create)
{
    CHECK(parent_pgd != NULL, "parent_pgd is NULL", return NULL;);
    CHECK(vpn_level < 512, "vpn_level is out of bounds", return NULL;);
    CHECK(va % PAGE_SIZE == 0, "va is not page aligned", return NULL;);
    
    pgtbl_t* child_pgd = NULL;
    if( (parent_pgd[vpn_level] & PTE_V) == 0)//验证对应子页表是否存在，
    {
        if(!create) 
        {
            return NULL;//如果不存在,但指明不需要创建就返回
        }
        else //否则创建对应子页表
        {
            child_pgd = (pgtbl_t*)page_alloc(1);
            if(child_pgd == NULL) return NULL;
            memset(child_pgd,0,PAGE_SIZE);
            //设置对应子页表的物理地址，并标记为有效（PTE_V
            parent_pgd[vpn_level] = PA2PTE(child_pgd) | PTE_V;
            return child_pgd;
        }
    }
    else //如果存在，直接返回对应pmd的物理地址
    {
       //返回对应pmd的物理地址
        return (pgtbl_t*)PTE2PA(parent_pgd[vpn_level]);
    }
}

/**
 * @brief 页表遍历函数
 *
 * 该函数根据给定的页全局目录指针（pgd）、虚拟地址（va）和是否创建页表项的布尔值（create），遍历页表并返回对应的页表项指针（pte_t*）。
 *
 * @param pgd 页全局目录指针
 * @param va 虚拟地址
 * @param create 是否创建页表项
 *
 * @return 对应的页表项指针（pte_t*），如果未找到对应的页表项，则返回NULL。
 */
pte_t* page_walk(pgtbl_t *pgd, uintptr_t va, bool create)
{
    CHECK(pgd != NULL, "pgd is NULL", return NULL;);
    CHECK(va % PAGE_SIZE == 0, "va is not page aligned", return NULL;);
   
    uintptr_t *pmd = NULL;
    uintptr_t *pte = NULL;

    u64 vpn2 = (va >> 30) & 0x1ff;
    u64 vpn1 = (va >> 21) & 0x1ff;
    u64 vpn0 = (va >> 12) & 0x1ff;

    pmd = get_child_pgtbl(pgd,vpn2,va,true);//获取对应pmd的物理地址
    if(pmd == NULL) return NULL;
    
    pte = get_child_pgtbl(pmd,vpn1,va,true);
    if (pte == NULL) return NULL;
    
    return (pte_t*)&pte[vpn0];
}


int mmap(pgtbl_t *pgd, uintptr_t vaddr, uintptr_t paddr, enum pgt_size page_size, u64 flags)
{
    CHECK(pgd != NULL, "pgd is NULL", return -1;);
    CHECK(vaddr % page_size == 0, "vaddr is not page aligned", return -1;);
    CHECK(paddr % page_size == 0, "paddr is not page aligned", return -1;);

    u64 vpn2 = (vaddr >> 30) & 0x1ff;
    u64 vpn1 = (vaddr >> 21) & 0x1ff;
    u64 vpn0 = (vaddr >> 12) & 0x1ff;

    switch (page_size)
    {
        case PAGE_SIZE_4K:
        {
            pgtbl_t *l1 = get_child_pgtbl(pgd, vpn2, vaddr, true);
            pgtbl_t *l0 = get_child_pgtbl(l1, vpn1, vaddr, true);
            pte_t *pte = &l0[vpn0];
            *pte = PA2PTE(paddr) | flags | PTE_V;
            break;
        }
            
        case PAGE_SIZE_2M:
        {
            pgtbl_t *l1 = get_child_pgtbl(pgd, vpn2, vaddr, true);
            pte_t *pte = &l1[vpn1];
            *pte = PA2PTE(paddr) | flags | PTE_V;
            break;
        }

        case PAGE_SIZE_1G:
        {
            pte_t *pte = &pgd[vpn2];
            *pte = PA2PTE(paddr) | flags | PTE_V;
            break;
        }

        default:
        {
            printk("Unsupported page size\n");
            return -1;
        }
    }
    return 0;
}

int map_range(pgtbl_t *pgd, uintptr_t vaddr, uintptr_t paddr, size_t size, u64 flags)
{
    CHECK(pgd != NULL, "pgd is NULL", return -1;);
    CHECK(vaddr % PAGE_SIZE_4K == 0, "vaddr is not page aligned", return -1;);
    CHECK(paddr % PAGE_SIZE_4K == 0, "paddr is not page aligned", return -1;);
    CHECK(size % PAGE_SIZE_4K == 0, "size is not page aligned", return -1;);
    
    uintptr_t va = vaddr;
    uintptr_t pa = paddr;
    uintptr_t end = vaddr + size;

    while (va < end) 
    {
        enum pgt_size chunk_size;

        // 能否用 1GB 大页
        if ((va % PAGE_SIZE_1G == 0) && (pa % PAGE_SIZE_1G == 0) && (end - va) >= PAGE_SIZE_1G) 
        {
            chunk_size = PAGE_SIZE_1G;
        }
        // 能否用 2MB 大页
        else if ((va % PAGE_SIZE_2M == 0) && (pa % PAGE_SIZE_2M == 0) && (end - va) >= PAGE_SIZE_2M) 
        {
            chunk_size = PAGE_SIZE_2M;
        }
        // 否则用 4KB
        else 
        {
            chunk_size = PAGE_SIZE_4K;
        }

        if (mmap(pgd, va, pa, chunk_size, flags) < 0) {
            return -1;
        }

        va += chunk_size;
        pa += chunk_size;
    }
    asm volatile("sfence.vma zero, zero"); // 刷新 TLB
    return 0;
}

void early_page_table_init()
{
    early_pgd = (pgtbl_t*)page_alloc(1);
    if(early_pgd == NULL) return;
    memset(early_pgd,0,PAGE_SIZE);

    // 恒等映射bootloader代码段，数据段，栈以及堆的保留页到虚拟地址空间 
    map_range(early_pgd,(uintptr_t)boot_text_start,(uintptr_t)boot_text_start,(size_t)boot_text_size,PTE_R | PTE_X);
    map_range(early_pgd,(uintptr_t)boot_rodata_start ,(uintptr_t)boot_rodata_start,(size_t)boot_rodata_size,PTE_R);
    map_range(early_pgd,(uintptr_t)boot_data_start ,(uintptr_t)boot_data_start,(size_t)boot_data_size,PTE_R | PTE_W);
    map_range(early_pgd,(uintptr_t)boot_bss_start ,(uintptr_t)boot_bss_start,(size_t)boot_bss_size,PTE_R | PTE_W);
    map_range(early_pgd,(uintptr_t)boot_heap_start,(uintptr_t)boot_heap_start,(size_t)boot_heap_size,PTE_R | PTE_W);
    map_range(early_pgd,(uintptr_t)boot_stack_start,(uintptr_t)boot_stack_start,(size_t)boot_stack_size*2,PTE_R | PTE_W);

    //恒等映射外设寄存器地址
    map_range(early_pgd,(uintptr_t)CLINT_BASE,(uintptr_t)CLINT_BASE,11*PAGE_SIZE,PTE_R | PTE_W);
    map_range(early_pgd,(uintptr_t)PLIC_BASE,(uintptr_t)PLIC_BASE,0x200*PAGE_SIZE,PTE_R | PTE_W);
    map_range(early_pgd,(uintptr_t)UART_BASE,(uintptr_t)UART_BASE,PAGE_SIZE,PTE_R | PTE_W);
    map_range(early_pgd,(uintptr_t)VIRTIO_MMIO_BASE,(uintptr_t)VIRTIO_MMIO_BASE,PAGE_SIZE,PTE_R | PTE_W);
    map_range(early_pgd,(uintptr_t)0x50000000,(uintptr_t)0x50000000,PAGE_SIZE,PTE_R);

    map_range(early_pgd,(uintptr_t)0x80200000,(uintptr_t)0x80200000,(size_t)0x7e00000,PTE_R | PTE_W | PTE_X);
    map_range(early_pgd,(uintptr_t)0xffffffffc0200000,(uintptr_t)0x80200000,(size_t)0x7e00000,PTE_R | PTE_W | PTE_X);
    
    //设置satp寄存器
    asm volatile("sfence.vma zero, zero");
    asm volatile("csrw satp,%0"::"r"(MAKE_SATP(early_pgd)));

    // printk("kernel page table init success!\n");
}

void vm_init()
{
    // 将整个用户空间（39位）设置保护
    pmpaddr0_w(0x3fffffffffffff);
    pmpcfg0_w(0xf);
}

