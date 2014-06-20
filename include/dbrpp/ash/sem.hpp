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
#ifndef DBRPP_ASH_SEM_HPP
#define DBRPP_ASH_SEM_HPP

#include <dbr/ash/sem.h>

namespace dbr {

/**
 * @addtogroup Thread
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
