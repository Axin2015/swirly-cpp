/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIG_ORDIDX_H
#define FIG_ORDIDX_H

// OrdIdx of orders by reference.

#include <dbr/ash/defs.h>
#include <dbr/ash/stack.h>

struct DbrOrder;

#ifndef FIG_ORDIDX_BUCKETS
#define FIG_ORDIDX_BUCKETS 257
#endif // FIG_ORDIDX_BUCKETS

struct FigOrdIdx {
    struct {
        struct DbrStack refs;
    } buckets[FIG_ORDIDX_BUCKETS];
};

DBR_EXTERN void
fig_ordidx_init(struct FigOrdIdx* ordidx);

static inline void
fig_ordidx_reset(struct FigOrdIdx* ordidx)
{
    fig_ordidx_init(ordidx);
}

DBR_EXTERN void
fig_ordidx_insert(struct FigOrdIdx* ordidx, struct DbrOrder* order);

DBR_EXTERN struct DbrOrder*
fig_ordidx_remove(struct FigOrdIdx* ordidx, DbrIden trid, const char* ref);

DBR_EXTERN struct DbrOrder*
fig_ordidx_find(const struct FigOrdIdx* ordidx, DbrIden trid, const char* ref);

#endif // FIG_ORDIDX_H
