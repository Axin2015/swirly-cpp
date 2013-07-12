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
#ifndef ELM_INDEX_H
#define ELM_INDEX_H

// Index of trader's orders by reference.

#include <dbr/defs.h>
#include <dbr/types.h>

#include <dbr/stack.h>

struct ElmIndex;

#ifndef ELM_INDEX_BUCKETS
#define ELM_INDEX_BUCKETS 257
#endif // ELM_INDEX_BUCKETS

struct ElmIndex {
    struct {
        struct DbrStack refs;
    } buckets[ELM_INDEX_BUCKETS];
};

DBR_EXTERN void
elm_index_init(struct ElmIndex* index);

DBR_EXTERN void
elm_index_insert(struct ElmIndex* index, struct DbrOrder* order);

DBR_EXTERN struct DbrOrder*
elm_index_remove(struct ElmIndex* index, DbrIden trid, const char* ref);

DBR_EXTERN struct DbrOrder*
elm_index_find(const struct ElmIndex* index, DbrIden trid, const char* ref);

#endif // ELM_INDEX_H
