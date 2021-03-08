#ifndef _NOS_SYNC_SPINLOCK_H
#define _NOS_SYNC_SPINLOCK_H

#include <stdint.h>
#include <stdbool.h>

struct spinlock {
  volatile uint32_t locked;
};

void spinlock_init(struct spinlock *);

void spinlock_lock(struct spinlock *);
void spinlock_unlock(struct spinlock *);

#endif  // _NOS_SYNC_SPINLOCK_H
