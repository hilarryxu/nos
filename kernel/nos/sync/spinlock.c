#include <nos/sync/spinlock.h>

// 原子操作
// 返回旧值的同时设置新值到内存地址中
static inline unsigned int
xchg(volatile unsigned int *addr, unsigned int newval)
{
  uint32_t result;

  // The + in "+m" denotes a read-modify-write operand.
  asm volatile("lock; xchgl %0, %1"
               : "+m"(*addr), "=a"(result)
               : "1"(newval)
               : "cc");

  return result;
}

void
spinlock_init(struct spinlock *lock)
{
  lock->locked = 0;
}

void
spinlock_lock(struct spinlock *lock)
{
  // 等待 locked 为 0，并将其置为 1
  while (xchg(&lock->locked, 1) != 0)
    ;

  __sync_synchronize();
}

void
spinlock_unlock(struct spinlock *lock)
{
  __sync_synchronize();

  asm volatile("movl $0, %0" : "+m"(lock->locked) :);
}
