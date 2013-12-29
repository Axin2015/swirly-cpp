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
#ifndef DBR_SLNODE_H
#define DBR_SLNODE_H

#include <assert.h>
#include <stddef.h> // NULL

/**
 * @addtogroup Node
 * @{
 */

/**
 * Singly-linked node.
 */
struct DbrSlNode {
    struct DbrSlNode* next;
};

#define DBR_SLNODE_INIT(n) { .next = NULL }

static inline void
dbr_slnode_init(struct DbrSlNode* node)
{
    assert(node);
    node->next = NULL;
}

static inline struct DbrSlNode*
dbr_slnode_next(struct DbrSlNode* node)
{
    assert(node);
    return node->next;
}

/** @} */

#endif // DBR_SLNODE_H
