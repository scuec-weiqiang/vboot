/**
 * @FilePath: /ZZZ/kernel/fs/vfs/vfs_pcache.h
 * @Description:  
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-09-01 17:20:59
 * @LastEditTime: 2025-09-03 22:26:47
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
*/
#ifndef VFS_PCACHE_H
#define VFS_PCACHE_H

#include "types.h"
#include "vfs_types.h"

extern int pcache_init(void);
extern void pcache_destory(void);
extern struct page *create_page(struct inode *inode, pgoff_t index) ;

extern struct page *pget(struct inode *inode, u32 index);
extern int pput(struct page *page);
extern int pcache_sync();

#endif