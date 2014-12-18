/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIG_SESSIDX_H
#define FIG_SESSIDX_H

#include <sc/ash/stack.h>
#include <sc/elm/types.h>

// OrdIdx of sessions by reference.

struct ScSess;

#ifndef FIG_SESSIDX_BUCKETS
#define FIG_SESSIDX_BUCKETS 257
#endif // FIG_SESSIDX_BUCKETS

struct FigSessIdx {
    ScPool pool;
    struct {
        struct ScStack uuids;
    } buckets[FIG_SESSIDX_BUCKETS];
};

SC_EXTERN void
fig_sessidx_init(struct FigSessIdx* sessidx, ScPool pool);

SC_EXTERN void
fig_sessidx_term(struct FigSessIdx* sessidx);

SC_EXTERN struct ScSess*
fig_sessidx_lazy(struct FigSessIdx* sessidx, const ScUuid uuid);

#endif // FIG_SESSIDX_H
