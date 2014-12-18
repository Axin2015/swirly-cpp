/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ASH_SEM_HPP
#define SCPP_ASH_SEM_HPP

#include <sc/ash/sem.h>

namespace sc {

/**
 * @addtogroup Sem
 * @{
 */

class Sem {
    mutable ScSem impl_;
public:
    ~Sem() noexcept
    {
        sc_sem_term(&impl_);
    }
    explicit
    Sem(int count = 0)
    {
        sc_sem_init(&impl_, count);
    }
    operator ScSem&() noexcept
    {
        return impl_;
    }
    ScSem*
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
        sc_sem_post(&impl_, n);
    }
    void
    wait(int n = 1)
    {
        sc_sem_wait(&impl_, n);
    }
};

/** @} */

} // sc

#endif // SCPP_ASH_SEM_HPP
