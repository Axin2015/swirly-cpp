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
#ifndef FIG_SESSIDX_H
#define FIG_SESSIDX_H

#include <dbr/stack.h>
#include <dbr/types.h>

// Index of sessions by reference.

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
