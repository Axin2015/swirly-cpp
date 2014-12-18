/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_REFCOUNT_H
#define SC_ASH_REFCOUNT_H

#include <sc/elm/pool.h>

/**
 * @addtogroup Exec
 * @{
 */

/**
 * Increment reference count to Exec.
 */

static inline void
sc_exec_incref(struct ScExec* exec)
{
    assert(exec->refs_ >= 1);
    ++exec->refs_;
}

/**
 * Decrement reference count to Exec. Exec is freed when reference count reaches zero.
 */

static inline void
sc_exec_decref(struct ScExec* exec, ScPool pool)
{
    assert(exec->refs_ >= 1);
    if (--exec->refs_ == 0)
        sc_pool_free_exec(pool, exec);
}

/** @} */

#endif // SC_ASH_REFCOUNT_H
