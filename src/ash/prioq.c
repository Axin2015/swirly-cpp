/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/ash/prioq.h>

#include <sc/ash/err.h>

#include <assert.h>
#include <stdlib.h> // malloc()

/*
    Heap invariant:

    If c is a child node of p, then p <= c.
    I.e. every node is less than or equal to its parent.

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

static inline ScBool
invariant(struct ScPrioq* pq, size_t c, size_t p)
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
swap(struct ScPrioq* pq, size_t i, size_t j)
{
    struct ScElem tmp = pq->elems[i];
    pq->elems[i] = pq->elems[j];
    pq->elems[j] = tmp;
}

static size_t
sift_up(struct ScPrioq* pq, size_t c)
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
sift_down(struct ScPrioq* pq, size_t p)
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

static ScBool
grow(struct ScPrioq* pq, size_t capacity)
{
    // One-based index.
    struct ScElem* elems = realloc(pq->elems, sizeof(struct ScElem) * (1 + capacity));
    if (!elems) {
        sc_err_set(SC_ENOMEM, "out of memory");
        return SC_FALSE;
    }
    // Commit.
    pq->capacity = capacity;
    pq->elems = elems;
    return SC_TRUE;
}

static inline ScBool
reserve(struct ScPrioq* pq, size_t capacity)
{
    // If desired capacity is less than or equal to the current capacity, the return true, otherwise
    // grow by 50%.
    return capacity <= pq->capacity ? SC_TRUE
        : grow(pq, sc_max(capacity, (pq->capacity * 3) / 2));
}

SC_API void
sc_prioq_term(struct ScPrioq* pq)
{
    free(pq->elems);
}

SC_API ScBool
sc_prioq_init(struct ScPrioq* pq)
{
    pq->size = 0;
    // Initial capacity.
    pq->capacity = 64;
    // One-based index.
    pq->elems = malloc(sizeof(struct ScElem) * (1 + pq->capacity));
    if (!pq->elems) {
        sc_err_set(SC_ENOMEM, "out of memory");
        return SC_FALSE;
    }
    return SC_TRUE;
}

SC_API ScBool
sc_prioq_reserve(struct ScPrioq* pq, size_t capacity)
{
    return reserve(pq, capacity);
}

SC_API ScBool
sc_prioq_push(struct ScPrioq* pq, ScIden id, ScKey key)
{
    if (!reserve(pq, pq->size + 1))
        return SC_FALSE;
    // Push back.
    pq->elems[++pq->size].id = id;
    pq->elems[pq->size].key = key;
    // Restore invariant.
    sift_up(pq, pq->size);
    return SC_TRUE;
}

SC_API void
sc_prioq_pop(struct ScPrioq* pq)
{
    assert(pq->size > 0);
    // Fill gap with last.
    pq->elems[1] = pq->elems[pq->size--];
    // Restore invariant.
    sift_down(pq, 1);
}

SC_API ScBool
sc_prioq_remove(struct ScPrioq* pq, ScIden id)
{
    if (pq->size == 0)
        return SC_FALSE;
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
            return SC_TRUE;
        }
    // Gap fill is not required when last is removed.
    if (pq->elems[i].id == id) {
        pq->size--;
        return SC_TRUE;
    }
    return SC_FALSE;
}

SC_API ScBool
sc_prioq_replace(struct ScPrioq* pq, ScIden id, ScKey key)
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
            return SC_TRUE;
        }
    return SC_FALSE;
}
