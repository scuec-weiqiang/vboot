/**
 * @FilePath: /vboot/vm.h
 * @Description:  
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-05-08 22:00:45
 * @LastEditTime: 2025-09-21 15:01:25
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
*/
#ifndef _VM_H
#define _VM_H

#include "mm.h"
#include "types.h"

enum pgt_size
{
    PAGE_SIZE_4K = 1UL << 12,
    PAGE_SIZE_2M = 1UL << 21,
    PAGE_SIZE_1G = 1UL << 30,
};

extern pgtbl_t* early_pgd;//kernel_page_global_directory 内核页全局目录

extern void vm_init();
extern pgtbl_t* get_child_pgtbl(pgtbl_t *parent_pgd, u64 vpn_level, u64 va, bool create);
extern pte_t* page_walk(pgtbl_t *pgd, uintptr_t va, bool create);
extern int mmap(pgtbl_t *pgd, uintptr_t vaddr, uintptr_t paddr, enum pgt_size page_size, u64 flags);

#endif