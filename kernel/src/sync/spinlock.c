#include <sync/spinlock.h>

#include <arch/arch.h>

void spinlock_lock(spinlock_t *lock)
{
    while (atomic_swap(&lock->val, 1))
    {
        if (lock->waiters)
        {
            atomic_inc(&lock->waiters);
        }

        while (lock->val)
        {
            // TODO: This should switch task
        }

        if (lock->waiters)
        {
            atomic_dec(&lock->waiters);
        }
    }
}

void spinlock_init(spinlock_t *lock)
{
    lock->val = 0;
    lock->waiters = 0;
}

void spinlock_unlock(spinlock_t *lock)
{
    if (lock->val)
    {
        atomic_store(&lock->val, 0);

        // This may be customized to check the priority of the process running
        if (lock->waiters)
        {
            // This should switch task
        }
    }
}