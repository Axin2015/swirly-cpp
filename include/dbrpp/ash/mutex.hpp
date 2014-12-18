/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
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
