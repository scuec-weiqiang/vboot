#include "symbols.h"
#include "string.h"
#include "types.h"

static uintptr_t alloc_pos;
static size_t free_size = 0;


void malloc_init()
{
    alloc_pos = heap_start; // 可分配内存的起始地址
    free_size = heap_size;  // 可分配内存的大小
}


void *malloc(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }
    if (size > free_size)
    {
        return NULL;
    }
    alloc_pos = (alloc_pos + 7) & (~0x7); // 8字节对齐
    void *p = (void *)alloc_pos;
    alloc_pos += size;
    free_size -= size;
    return p;
}


void *page_malloc(size_t npages)
{
    size_t size = npages * 4096;
    if (size == 0)
    {
        return NULL;
    }
    if (size > free_size)
    {
        return NULL;
    }
    alloc_pos = (alloc_pos + 4095) & (~0xFFF); // 4k字节对齐
    void *p = (void *)alloc_pos;
    alloc_pos += size;
    free_size -= size;
    return p;
}

void free(void *ptr)
{
    // 引导阶段不支持释放内存
    (void)ptr;
}
