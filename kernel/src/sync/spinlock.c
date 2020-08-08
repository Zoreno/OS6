/**
 * @file spinlock.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 * 
 * @brief 
 * 
 * @copyright Copyright (C) 2019,
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 * 
 */

#include <sync/spinlock.h>

#include <arch/arch.h>

#include <process/process.h>

void spinlock_lock(spinlock_t *lock)
{

#if 0

	while (atomic_swap(&lock->val, 1))
	{
		if (lock->waiters)
		{
			atomic_inc(&lock->waiters);
		}

		while (lock->val)
		{
			process_yield(1);
		}

		if (lock->waiters)
		{
			atomic_dec(&lock->waiters);
		}
	}

#else
	while (1)
	{
		// TODO: Look into this. This looks a bit wierd.
		if (!xchg_64(&lock->val, (void *)1))
		{
			return;
		}

		while (lock->val)
		{
			process_yield(1);
		}
	}

#endif
}

void *spinlock_trylock(spinlock_t *lock)
{
	return xchg_64(&lock->val, (void *)1);
}

void spinlock_init(spinlock_t *lock)
{
	lock->val = 0;
	lock->waiters = 0;
}

void spinlock_unlock(spinlock_t *lock)
{
#if 0

	if (lock->val)
	{
		atomic_store(&lock->val, 0);

		// This may be customized to check the priority of the process running
		if (lock->waiters)
		{
			//process_yield(1);
		}
	}

#else

	BARRIER;
	lock->val = 0;

#endif
}