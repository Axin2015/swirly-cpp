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
#ifndef FIG_ORDIDX_H
#define FIG_ORDIDX_H

// Index of orders by reference.

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
