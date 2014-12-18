/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ASH_SEM_HPP
#define DBRPP_ASH_SEM_HPP

#include <dbr/ash/sem.h>

namespace dbr {

/**
 * @addtogroup Sem
 * @{
 */

class Sem {
    mutable DbrSem impl_;
public:
    ~Sem() noexcept
    {
        dbr_sem_term(&impl_);
    }
    explicit
    Sem(int count = 0)
    {
        dbr_sem_init(&impl_, count);
    }
    operator DbrSem&() noexcept
    {
        return impl_;
    }
    DbrSem*
    c_arg() noexcept
    {
        return &impl_;
    }

    // Copy semantics.

    Sem(const Sem&) = delete;

    Sem&
    operator =(const Sem&) = delete;

    void
    post(int n = 1)
    {
        dbr_sem_post(&impl_, n);
    }
    void
    wait(int n = 1)
    {
        dbr_sem_wait(&impl_, n);
    }
};

/** @} */

} // dbr

#endif // DBRPP_ASH_SEM_HPP
