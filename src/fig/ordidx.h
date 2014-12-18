/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIG_ORDIDX_H
#define FIG_ORDIDX_H

// OrdIdx of orders by reference.

#include <sc/ash/defs.h>
#include <sc/ash/stack.h>

struct ScOrder;

#ifndef FIG_ORDIDX_BUCKETS
#define FIG_ORDIDX_BUCKETS 257
#endif // FIG_ORDIDX_BUCKETS

struct FigOrdIdx {
    struct {
        struct ScStack refs;
    } buckets[FIG_ORDIDX_BUCKETS];
};

SC_EXTERN void
fig_ordidx_init(struct FigOrdIdx* ordidx);

static inline void
fig_ordidx_reset(struct FigOrdIdx* ordidx)
{
    fig_ordidx_init(ordidx);
}

SC_EXTERN void
fig_ordidx_insert(struct FigOrdIdx* ordidx, struct ScOrder* order);

SC_EXTERN struct ScOrder*
fig_ordidx_remove(struct FigOrdIdx* ordidx, ScIden trid, const char* ref);

SC_EXTERN struct ScOrder*
fig_ordidx_find(const struct FigOrdIdx* ordidx, ScIden trid, const char* ref);

#endif // FIG_ORDIDX_H
