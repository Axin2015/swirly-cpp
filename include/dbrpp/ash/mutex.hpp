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
#ifndef DBRPP_ASH_MUTEX_HPP
#define DBRPP_ASH_MUTEX_HPP

#include <pthread.h>

namespace dbr {

/**
 * @addtogroup Mutex
 * @{
 */

class Mutex {
    friend class Lock;
    pthread_mutex_t impl_;
public:
    ~Mutex() noexcept
    {
        pthread_mutex_destroy(&impl_);
    }
    Mutex()
    {
        pthread_mutex_init(&impl_, NULL);
    }
};

class Lock {
    Mutex& mutex_;
public:
    ~Lock() noexcept
    {
        pthread_mutex_unlock(&mutex_.impl_);
    }
    explicit
    Lock(Mutex& mutex)
        : mutex_(mutex)
    {
        pthread_mutex_lock(&mutex.impl_);
    }
};

/** @} */

} // dbr

#endif // DBRPP_ASH_MUTEX_HPP
