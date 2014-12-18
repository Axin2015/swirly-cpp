/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_REFCOUNT_H
#define DBR_ASH_REFCOUNT_H

#include <dbr/elm/pool.h>

/**
 * @addtogroup Exec
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

#endif // DBR_ASH_REFCOUNT_H
