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
#ifndef FIG_INDEX_H
#define FIG_INDEX_H

// Index of orders by reference.

#include <dbr/ash/defs.h>
#include <dbr/ash/stack.h>

struct DbrOrder;

#ifndef FIG_INDEX_BUCKETS
#define FIG_INDEX_BUCKETS 257
#endif // FIG_INDEX_BUCKETS

struct FigIndex {
    struct {
        struct DbrStack refs;
    } buckets[FIG_INDEX_BUCKETS];
};

DBR_EXTERN void
fig_index_init(struct FigIndex* index);

static inline void
fig_index_reset(struct FigIndex* index)
{
    fig_index_init(index);
}

DBR_EXTERN void
fig_index_insert(struct FigIndex* index, struct DbrOrder* order);

DBR_EXTERN struct DbrOrder*
fig_index_remove(struct FigIndex* index, DbrIden trid, const char* ref);

DBR_EXTERN struct DbrOrder*
fig_index_find(const struct FigIndex* index, DbrIden trid, const char* ref);

#endif // FIG_INDEX_H
