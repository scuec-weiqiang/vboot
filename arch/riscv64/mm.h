/**
 * @FilePath: /vboot/arch/riscv64/mm.h
 * @Description:  
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-09-20 15:39:46
 * @LastEditTime: 2025-09-21 16:56:41
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
*/

#ifndef _MM_H
#define _MM_H

#include "types.h"

#define PTE_V (1 << 0)      // 有效位
#define PTE_R (1 << 1)      // 可读
#define PTE_W (1 << 2)      // 可写
#define PTE_X (1 << 3)      // 可执行
#define PTE_U (1 << 4)      // 用户模式可访问

#define PAGE_SIZE 0x1000    // 页大小4096字节
#define PAGE_SHIFT 12     // 页偏移量
#define PAGE_MASK (~(PAGE_SIZE - 1))   // 页掩码

#define PTE_FLAGS (PTE_V | PTE_R | PTE_W | PTE_X)   // 页表项标志位

#define PA2PTE(pa) ((((u64)(pa) >> 12) << 10))
#define PTE2PA(pte) (((pte&0xffffffffffffffff) >> 10) << 12)

#define SATP_SV39 (8L << 60)
#define SATP_MODE SATP_SV39 
#define MAKE_SATP(pagetable) (SATP_MODE | (((u64)pagetable) >> 12))

#define HIGH_VA 0xffffffffc0000000
#define MAKE_HIGH_VA(pa) (HIGH_VA + ((u64)(pa)) - 0x80000000)

#define MMIO_BASE 0xffffffdf00000000
#define MAKE_MMIO_VA(pa) (MMIO_BASE + ((u64)(pa)))



typedef u64 pte_t;
typedef pte_t pgtbl_t;

#endif