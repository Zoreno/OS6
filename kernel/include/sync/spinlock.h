#ifndef _SPINLOCK_H
#define _SPINLOCK_H

typedef struct
{
    long long int val;
    long long int waiters;
} spinlock_t;

void spinlock_lock(spinlock_t *lock);
void spinlock_init(spinlock_t *lock);
void spinlock_unlock(spinlock_t *lock);

#endif