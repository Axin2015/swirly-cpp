/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
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
#include <dbr/prioq.h>

#include <dbr/err.h>

#include <assert.h>
#include <stdlib.h> // malloc()

enum { INVAL = -1 };

/*
    Heap invariant:

    If c is a child node of p, then p <= c.
    I.e. every node is greater than or equal to its parent.

      1234567
      1327465

         1
        / \
      3     2
     / \   / \
    7   4 6   5

 */

static inline DbrBool
invariant(struct DbrPrioq* pq, size_t c, size_t p)
{
    return pq->elems[p].key <= pq->elems[c].key;
}

static inline size_t
left_child(size_t i)
{
    // i * 2
    return i << 1;
}

static inline size_t
parent(size_t i)
{
    // i / 2
    return i >> 1;
}

static inline void
swap(struct DbrPrioq* pq, size_t i, size_t j)
{
    struct DbrPair tmp = pq->elems[i];
    pq->elems[i] = pq->elems[j];
    pq->elems[j] = tmp;
}

static void
sift_up(struct DbrPrioq* pq)
{
    // heap(1, n - 1)
    size_t c, p;
    // While child is not root.
    for (c = pq->size; c != 1; c = p) {
        p = parent(c);
        if (invariant(pq, c, p))
            break;
        // Restore invariant.
        swap(pq, c, p);
    }
    // heap(1, n)
}

static void
sift_down(struct DbrPrioq* pq)
{
    // heap(2, n)
    const size_t n = pq->size;
    size_t c, p;
    // While parent has child.
    for (p = 1; (c = left_child(p)) && c <= n; p = c) {
        // Use child with lower value.
        const size_t r = c + 1;
        if (r <= n && pq->elems[r].key < pq->elems[c].key)
            c = r;
        if (invariant(pq, c, p))
            break;
        // Restore invariant.
        swap(pq, c, p);
    }
    // heap(1, n)
}

static void
flush(struct DbrPrioq* pq)
{
    // While not empty.
    while (pq->size > 0) {
        // Done if top has valid id.
        if (pq->elems[1].id != INVAL)
            break;
        // Fill gap with last.
        pq->elems[1] = pq->elems[pq->size--];
        // Restore invariant.
        // heap(2, n)
        sift_down(pq);
        // heap(1, n)
    }
}

static DbrBool
grow(struct DbrPrioq* pq)
{
    const size_t capacity = pq->capacity * 2;
    // One-based index.
    struct DbrPair* elems = realloc(pq->elems, sizeof(struct DbrPair) * (1 + capacity));
    if (!elems) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return DBR_FALSE;
    }
    // Commit.
    pq->capacity = capacity;
    pq->elems = elems;
    return DBR_TRUE;
}

static void
pop(struct DbrPrioq* pq)
{
    assert(pq->size > 0);
    // Fill gap with last.
    pq->elems[1] = pq->elems[pq->size--];
    // Restore invariant.
    // heap(2, n)
    sift_down(pq);
    // heap(1, n)
    flush(pq);
}

DBR_API void
dbr_prioq_term(struct DbrPrioq* pq)
{
    free(pq->elems);
}

DBR_API DbrBool
dbr_prioq_init(struct DbrPrioq* pq)
{
    pq->size = 0;
    // Initial capacity.
    pq->capacity = 64;
    // One-based index.
    pq->elems = malloc(sizeof(struct DbrPair) * (1 + pq->capacity));
    if (!pq->elems) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

DBR_API DbrBool
dbr_prioq_clear(struct DbrPrioq* pq, DbrIden id)
{
    assert(id != INVAL);
    if (pq->size > 0) {
        // If top element matches then pop and flush.
        if (pq->elems[1].id == id) {
            pop(pq);
            return DBR_TRUE;
        }
        // Linear search on remaining.
        for (size_t i = 2; i <= pq->size; ++i)
            if (pq->elems[i].id == id) {
                // Clear some element other than top.
                pq->elems[i].id = INVAL;
                return DBR_TRUE;
            }
    }
    return DBR_FALSE;
}

DBR_API DbrBool
dbr_prioq_push(struct DbrPrioq* pq, DbrKey key, DbrIden id)
{
    assert(id != INVAL);
    if (pq->capacity <= pq->size && !grow(pq))
        return DBR_FALSE;
    // Push back.
    pq->elems[++pq->size].key = key;
    pq->elems[pq->size].id = id;
    // Restore invariant.
    // heap(1, n - 1)
    sift_up(pq);
    // heap(1, n)
    return DBR_TRUE;
}

DBR_API void
dbr_prioq_pop(struct DbrPrioq* pq)
{
    pop(pq);
}
