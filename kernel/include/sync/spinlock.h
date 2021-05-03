/**
 * @file spinlock.h
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

#ifndef _SPINLOCK_H
#define _SPINLOCK_H

typedef struct
{
    long long int val;
    long long int waiters;
} spinlock_t;

void spinlock_lock(spinlock_t *lock);
void *spinlock_trylock(spinlock_t *lock);
void spinlock_init(spinlock_t *lock);
void spinlock_unlock(spinlock_t *lock);

#endif

//=============================================================================
// End of file
//=============================================================================
