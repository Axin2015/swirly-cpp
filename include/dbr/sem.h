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
#ifndef DBR_SEM_H
#define DBR_SEM_H

#include <pthread.h>

/**
 * @addtogroup Thread
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

#endif // DBR_SEM_H
