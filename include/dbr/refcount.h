/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#ifndef DBR_REFCOUNT_H
#define DBR_REFCOUNT_H

#include <dbr/pool.h>
#include <dbr/types.h>

/**
 * @addtogroup TypesExec
 * @{
 */

/**
 * Increment reference count to Exec.
 */

static inline void
dbr_exec_incref(struct DbrExec* exec)
{
    assert(exec->refs_ >= 1);
    ++exec->refs_;
}

/**
 * Decrement reference count to Exec. Exec is freed when reference count reaches zero.
 */

static inline void
dbr_exec_decref(struct DbrExec* exec, DbrPool pool)
{
    assert(exec->refs_ >= 1);
    if (--exec->refs_ == 0)
        dbr_pool_free_exec(pool, exec);
}

/** @} */

#endif // DBR_REFCOUNT_H
