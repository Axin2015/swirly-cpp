/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ASH_MUTEX_HPP
#define SCPP_ASH_MUTEX_HPP

#include <pthread.h>

namespace sc {

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

} // sc

#endif // SCPP_ASH_MUTEX_HPP
