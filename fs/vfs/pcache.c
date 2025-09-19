/**
 * @FilePath: /ZZZ/kernel/fs/vfs/vfs_pcache.c
 * @Description:
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-09-01 16:25:09
 * @LastEditTime: 2025-09-07 22:41:23
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
 */
#include "types.h"
#include "container_of.h"
#include "lru.h"
#include "spinlock.h"
#include "vfs_types.h"
#include "check.h"
#include "string.h"
#include "malloc.h"

struct lru_cache *global_page_cache = NULL;

static void lock(struct page *page)
{
    spin_lock(&(page->lock));
}

static void unlock(struct page *page)
{
    spin_unlock(&(page->lock));
}

static struct page *create_page(struct inode *inode, pgoff_t index)
{
    struct page *p = (struct page*)malloc(sizeof(struct page));
    CHECK(p != NULL, "Memory allocation for page failed", return NULL;);
    memset(p, 0, sizeof(struct page));
    p->data = malloc(VFS_PAGE_SIZE);
    CHECK(p->data != NULL, "Memory allocation for page data failed", free(p); return NULL;);

    p->lock.lock = 0;

    lock(p);
    p->inode = inode;
    p->index = index;
    p->uptodate = false;
    p->dirty = false;
    p->under_io = true; // 占位态, 表示正在加载
    lru_node_init(&p->p_lru_cache_node);
    hlist_node_init(&p->self_cache_node);
    unlock(p);

    return p;
}

static int destroy_page(struct page *page)
{
    if (page->data)
        free(page->data);
    free(page);
    return 0;
}

static hval_t vfs_global_page_lru_hash(const struct hlist_node *node)
{
    struct page *page = container_of(node, struct page, p_lru_cache_node);
    uintptr_t sb_ptr = (uintptr_t)page->inode;

    // 使用黄金比例相关的魔数
    const u32 golden_ratio = 0x9E3779B9U;

    // 混合两个值
    hval_t hash = (hval_t)sb_ptr * golden_ratio;
    hash ^= (hval_t)page->index * golden_ratio;

    // 再次混合以增强分布
    hash = hash ^ (hash >> 16);

    return hash;
}
static int vfs_global_page_lru_compare(const struct hlist_node *a, const struct hlist_node *b)
{
    struct page *page_a = container_of(a, struct page, p_lru_cache_node);
    struct page *page_b = container_of(b, struct page, p_lru_cache_node);

    if (page_a->inode == page_b->inode && page_a->index == page_b->index)
    {
        return 0; // 相等
    }
    return 1; // 不相等
}
static int vfs_global_page_lru_free(struct lru_node *node)
{
    CHECK(node != NULL, "vfs_lru_free: node is NULL", return -1;);

    struct page *page = container_of(node, struct page, p_lru_cache_node);
    return destroy_page(page);
}

int pcache_init()
{
    global_page_cache = lru_init(128, vfs_global_page_lru_free, vfs_global_page_lru_hash, vfs_global_page_lru_compare);
    CHECK(global_page_cache != NULL, "Failed to create page LRU cache", return -1;);
    return 0;
}

void pcache_destory()
{
    lru_destroy(global_page_cache);
}

struct page *pget(struct inode *inode, u32 index)
{
    struct page page;
    page.inode = inode;
    page.index = index;
    page.lock.lock = 0;

    struct page *found_page = NULL;
    // 1. 在 page cache 里查找 (基于 inode + index 的哈希)
    struct hlist_node *node = hashtable_lookup(inode->i_mapping->page_cache, &page.self_cache_node);
    if (node)
    {
        found_page = container_of(node, struct page, self_cache_node);
        return found_page;
    }

    // 2. 没有的话分配新页
    found_page = create_page(inode, index);

    // 3. 调用文件系统的 readpage，把数据读进来
    inode->i_mapping->a_ops->readpage(found_page);

    // 4. 插入缓存
    hashtable_insert(inode->i_mapping->page_cache, &found_page->self_cache_node);
    lru_hash_insert(global_page_cache, &found_page->p_lru_cache_node);
    lru_ref(global_page_cache, &found_page->p_lru_cache_node); // 引用计数+1，表示正在使用

    return found_page;
}

int pput(struct page *page)
{
    CHECK(page != NULL, "pput: page is NULL", return -1;);
    lock(page);
    lru_unref(global_page_cache, &page->p_lru_cache_node); // 引用计数-1
    if (page->p_lru_cache_node.ref_count == 0)
    {
        hashtable_remove(page->inode->i_mapping->page_cache, &page->self_cache_node);
    }
    unlock(page);
    return 0;
}

int vfs_pcache_sync_func(struct lru_cache *cache, struct lru_node *node)
{
    struct page *page = container_of(node, struct page, p_lru_cache_node);
    lock(page);
    if (page->dirty && page->inode->i_mapping->a_ops->writepage)
    {
        int ret = page->inode->i_mapping->a_ops->writepage(page);
        CHECK(ret >= 0, "pcache_sync: writepage failed", );
        page->dirty = false;
    }
    unlock(page);
    return 0;
}

int pcache_sync()
{
    CHECK(global_page_cache != NULL, "pcache_sync: global_page_cache is NULL", return -1;);
    lru_walk(global_page_cache, vfs_pcache_sync_func);
    return 0;
}