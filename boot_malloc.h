/**
 * @FilePath: /vboot/page_alloc.h
 * @Description:  
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-04-15 17:26:52
 * @LastEditTime: 2025-09-18 00:01:17
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
*/
#ifndef BOOT_MALLOC_H
#define BOOT_MALLOC_H

#include "types.h"

extern void malloc_init();
extern void *malloc(size_t size);
extern void *page_malloc(size_t npages);
extern void free(void *ptr);
#endif
