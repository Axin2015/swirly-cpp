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
#include <dbr/prioq.h>

#include <dbr/err.h>

#include <assert.h>
#include <stdlib.h> // malloc()

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

    Complete Example:

    push 6:

         6
        / \
      X     X
     / \   / \
    X   X X   X

    push 4:

         4
        / \
      6     X
     / \   / \
    X   X X   X

    push 3:

         3
        / \
      6     4
     / \   / \
    X   X X   X

    push 5:

         3
        / \
      5     4
     / \   / \
    6   X X   X

    push 7:

         3
        / \
      5     4
     / \   / \
    6   7 X   X

    replace 5 with 8:

         3
        / \
      6     4
     / \   / \
    8   7 X   X

    replace 6 with 2:

         2
        / \
      3     4
     / \   / \
    8   7 X   X

    pop:

         3
        / \
      7     4
     / \   / \
    8   X X   X

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
    struct DbrElem tmp = pq->elems[i];
    pq->elems[i] = pq->elems[j];
    pq->elems[j] = tmp;
}

static size_t
sift_up(struct DbrPrioq* pq, size_t c)
{
    size_t p;
    // While child is not root.
    for (; c != 1; c = p) {
        p = parent(c);
        if (invariant(pq, c, p))
            break;
        // Restore invariant.
        swap(pq, c, p);
    }
    return c;
}

static size_t
sift_down(struct DbrPrioq* pq, size_t p)
{
    const size_t n = pq->size;
    size_t c;
    // While parent has child.
    for (; (c = left_child(p)) && c <= n; p = c) {
        // Use child with lower value.
        const size_t r = c + 1;
        if (r <= n && pq->elems[r].key < pq->elems[c].key)
            c = r;
        if (invariant(pq, c, p))
            break;
        // Restore invariant.
        swap(pq, c, p);
    }
    return p;
}

static DbrBool
grow(struct DbrPrioq* pq, size_t capacity)
{
    // One-based index.
    struct DbrElem* elems = realloc(pq->elems, sizeof(struct DbrElem) * (1 + capacity));
    if (!elems) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return DBR_FALSE;
    }
    // Commit.
    pq->capacity = capacity;
    pq->elems = elems;
    return DBR_TRUE;
}

static inline DbrBool
reserve(struct DbrPrioq* pq, size_t capacity)
{
    // If desired capacity is less than or equal to the current capacity, the return true, otherwise
    // grow by 50%.
    return capacity <= pq->capacity ? DBR_TRUE
        : grow(pq, dbr_max(capacity, (pq->capacity * 3) / 2));
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
    pq->elems = malloc(sizeof(struct DbrElem) * (1 + pq->capacity));
    if (!pq->elems) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

DBR_API DbrBool
dbr_prioq_reserve(struct DbrPrioq* pq, size_t capacity)
{
    return reserve(pq, capacity);
}

DBR_API DbrBool
dbr_prioq_push(struct DbrPrioq* pq, DbrKey key, DbrIden id)
{
    if (!reserve(pq, pq->size + 1))
        return DBR_FALSE;
    // Push back.
    pq->elems[++pq->size].id = id;
    pq->elems[pq->size].key = key;
    // Restore invariant.
    sift_up(pq, pq->size);
    return DBR_TRUE;
}

DBR_API void
dbr_prioq_pop(struct DbrPrioq* pq)
{
    assert(pq->size > 0);
    // Fill gap with last.
    pq->elems[1] = pq->elems[pq->size--];
    // Restore invariant.
    sift_down(pq, 1);
}

DBR_API DbrBool
dbr_prioq_remove(struct DbrPrioq* pq, DbrIden id)
{
    if (pq->size == 0)
        return DBR_FALSE;
    // Linear search all except last.
    size_t i;
    for (i = 1; i < pq->size; ++i)
        if (pq->elems[i].id == id) {
            // Fill gap with last.
            pq->elems[i] = pq->elems[pq->size--];
            // Restore invariant.
            // Shift down if not shifted up.
            if (sift_up(pq, i) == i)
                sift_down(pq, i);
            return DBR_TRUE;
        }
    // Gap fill is not required when last is removed.
    if (pq->elems[i].id == id) {
        pq->size--;
        return DBR_TRUE;
    }
    return DBR_FALSE;
}

DBR_API DbrBool
dbr_prioq_replace(struct DbrPrioq* pq, DbrKey key, DbrIden id)
{
    // Linear search.
    for (size_t i = 1; i <= pq->size; ++i)
        if (pq->elems[i].id == id) {
            // Update key.
            pq->elems[i].key = key;
            // Restore invariant.
            // Shift down if not shifted up.
            if (sift_up(pq, i) == i)
                sift_down(pq, i);
            return DBR_TRUE;
        }
    return DBR_FALSE;
}
