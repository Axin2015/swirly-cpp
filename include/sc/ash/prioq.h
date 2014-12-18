/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_PRIOQ_H
#define SC_ASH_PRIOQ_H

/**
 * @file
 * Priority queue.
 */

#include <sc/ash/defs.h>

#include <stddef.h> // size_t

/**
 * @addtogroup Prioq
 * @{
 */

struct ScElem {
    ScIden id;
    ScKey key;
};

struct ScPrioq {
    size_t size;
    size_t capacity;
    struct ScElem* elems;
};

SC_API void
sc_prioq_term(struct ScPrioq* pq);

SC_API ScBool
sc_prioq_init(struct ScPrioq* pq);

static inline void
sc_prioq_reset(struct ScPrioq* pq)
{
    pq->size = 0;
}

SC_API ScBool
sc_prioq_reserve(struct ScPrioq* pq, size_t capacity);

/**
 * Push new element and return pointer to it.
 */

SC_API ScBool
sc_prioq_push(struct ScPrioq* pq, ScIden id, ScKey key);

SC_API void
sc_prioq_pop(struct ScPrioq* pq);

SC_API ScBool
sc_prioq_remove(struct ScPrioq* pq, ScIden id);

SC_API ScBool
sc_prioq_replace(struct ScPrioq* pq, ScIden id, ScKey key);

static inline const struct ScElem*
sc_prioq_top(struct ScPrioq* pq)
{
    // Root has lowest value.
    return pq->size > 0 ? &pq->elems[1] : NULL;
}

static inline size_t
sc_prioq_size(struct ScPrioq* pq)
{
    return pq->size;
}

static inline size_t
sc_prioq_capacity(struct ScPrioq* pq)
{
    return pq->capacity;
}

/** @} */

#endif // SC_ASH_PRIOQ_H
