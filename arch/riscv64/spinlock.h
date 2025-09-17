#ifndef RISCV_SPINLOCK_H
#define RISCV_SPINLOCK_H

struct spinlock
{
    volatile int lock;
};

#define SPINLOCK_INIT {0}

static inline void spinlock_init(struct spinlock *lock)
{
    lock->lock = 0;
}

/**
 * @brief 自旋锁函数
 *
 * 尝试获取自旋锁。如果锁已经被其他线程持有，则当前线程将自旋等待直到锁被释放。
 *
 * @param lock 自旋锁指针
 */
static inline void spin_lock(struct spinlock *lock)
{
    int value = 1;
    do
    {
        asm volatile(
            "amoswap.w.aq %0, %1, (%2)"
            : "=r"(value)
            : "r"(value), "r"(&lock->lock)
            : "memory"
        );
    } while (value != 0);
}

/**
 * @brief 解除自旋锁
 *
 * 使用原子操作解除自旋锁，确保多线程环境下的线程安全。
 *
 * @param lock 指向自旋锁对象的指针
 */
static inline void spin_unlock(struct spinlock *lock)
{
    asm volatile(
        "amoswap.w.rl zero,zero,(%0)"
        :
        : "r"(&lock->lock)
        : "memory"
    );
}

#endif