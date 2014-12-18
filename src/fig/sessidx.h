/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIG_SESSIDX_H
#define FIG_SESSIDX_H

#include <dbr/ash/stack.h>
#include <dbr/elm/types.h>

// OrdIdx of sessions by reference.

struct DbrSess;

#ifndef FIG_SESSIDX_BUCKETS
#define FIG_SESSIDX_BUCKETS 257
#endif // FIG_SESSIDX_BUCKETS

struct FigSessIdx {
    DbrPool pool;
    struct {
        struct DbrStack uuids;
    } buckets[FIG_SESSIDX_BUCKETS];
};

DBR_EXTERN void
fig_sessidx_init(struct FigSessIdx* sessidx, DbrPool pool);

DBR_EXTERN void
fig_sessidx_term(struct FigSessIdx* sessidx);

DBR_EXTERN struct DbrSess*
fig_sessidx_lazy(struct FigSessIdx* sessidx, const DbrUuid uuid);

#endif // FIG_SESSIDX_H
