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
#ifndef DBR_PRIOQ_H
#define DBR_PRIOQ_H

#include <dbr/defs.h>

#include <stddef.h> // size_t

/**
 * @addtogroup Prioq
 * @{
 */

struct DbrElem {
    DbrIden id;
    DbrKey key;
};

struct DbrPrioq {
    size_t size;
    size_t capacity;
    struct DbrElem* elems;
};

DBR_API void
dbr_prioq_term(struct DbrPrioq* pq);

DBR_API DbrBool
dbr_prioq_init(struct DbrPrioq* pq);

static inline void
dbr_prioq_reset(struct DbrPrioq* pq)
{
    pq->size = 0;
}

DBR_API DbrBool
dbr_prioq_reserve(struct DbrPrioq* pq, size_t capacity);

/**
 * Push new element and return pointer to it.
 */

DBR_API DbrBool
dbr_prioq_push(struct DbrPrioq* pq, DbrIden id, DbrKey key);

DBR_API void
dbr_prioq_pop(struct DbrPrioq* pq);

DBR_API DbrBool
dbr_prioq_remove(struct DbrPrioq* pq, DbrIden id);

DBR_API DbrBool
dbr_prioq_replace(struct DbrPrioq* pq, DbrIden id, DbrKey key);

static inline const struct DbrElem*
dbr_prioq_top(struct DbrPrioq* pq)
{
    // Root has lowest value.
    return pq->size > 0 ? &pq->elems[1] : NULL;
}

static inline size_t
dbr_prioq_size(struct DbrPrioq* pq)
{
    return pq->size;
}

static inline size_t
dbr_prioq_capacity(struct DbrPrioq* pq)
{
    return pq->capacity;
}

/** @} */

#endif // DBR_PRIOQ_H
