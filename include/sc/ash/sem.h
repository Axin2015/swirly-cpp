/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_SEM_H
#define SC_ASH_SEM_H

#include <pthread.h>

/**
 * @addtogroup Sem
 * @{
 */

struct ScSem {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
};

#define SC_SEM_INIT(s, count) {        \
    .mutex = PTHREAD_MUTEX_INITIALIZER, \
    .cond = PTHREAD_COND_INITIALIZER    \
    .count = count }

static inline void
sc_sem_init(struct ScSem* sem, int count)
{
    pthread_mutex_init(&sem->mutex, NULL);
    pthread_cond_init(&sem->cond, NULL);
    sem->count = count;
}

static inline void
sc_sem_term(struct ScSem* sem)
{
    pthread_cond_destroy(&sem->cond);
    pthread_mutex_destroy(&sem->mutex);
}

static inline void
sc_sem_post(struct ScSem* sem, int n)
{
    pthread_mutex_lock(&sem->mutex);
    sem->count += n;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
}

static inline void
sc_sem_wait(struct ScSem* sem, int n)
{
    pthread_mutex_lock(&sem->mutex);
    while (sem->count < n)
        pthread_cond_wait(&sem->cond, &sem->mutex);
    sem->count -= n;
    pthread_mutex_unlock(&sem->mutex);
}

/** @} */

#endif // SC_ASH_SEM_H
