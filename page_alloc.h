/**
 * @FilePath: /vboot/page_alloc.h
 * @Description:  
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-04-15 17:26:52
 * @LastEditTime: 2025-09-18 00:01:17
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
*/
#ifndef _PAGE_ALLOC_H
#define _PAGE_ALLOC_H

#include "types.h"
#include "mm.h"

#define RESERVED_PAGE_NUM           8
#define RESERVED_PAGE_SIZE          RESERVED_PAGE_NUM*PAGE_SIZE 

extern void page_alloc_init();
extern void* page_alloc(u64 npages);
extern void page_free(void* p);
extern void* malloc(size_t size);
extern void *kmalloc(size_t size);
extern void free(void* p);
extern void print_page(u64 start,u64 end);
extern u64 page_get_remain_mem();

#endif
