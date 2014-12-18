/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_SEM_H
#define DBR_ASH_SEM_H

#include <pthread.h>

/**
 * @addtogroup Sem
 * @{
 */

struct DbrSem {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
};

#define DBR_SEM_INIT(s, count) {        \
    .mutex = PTHREAD_MUTEX_INITIALIZER, \
    .cond = PTHREAD_COND_INITIALIZER    \
    .count = count }

static inline void
dbr_sem_init(struct DbrSem* sem, int count)
{
    pthread_mutex_init(&sem->mutex, NULL);
    pthread_cond_init(&sem->cond, NULL);
    sem->count = count;
}

static inline void
dbr_sem_term(struct DbrSem* sem)
{
    pthread_cond_destroy(&sem->cond);
    pthread_mutex_destroy(&sem->mutex);
}

static inline void
dbr_sem_post(struct DbrSem* sem, int n)
{
    pthread_mutex_lock(&sem->mutex);
    sem->count += n;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
}

static inline void
dbr_sem_wait(struct DbrSem* sem, int n)
{
    pthread_mutex_lock(&sem->mutex);
    while (sem->count < n)
        pthread_cond_wait(&sem->cond, &sem->mutex);
    sem->count -= n;
    pthread_mutex_unlock(&sem->mutex);
}

/** @} */

#endif // DBR_ASH_SEM_H
