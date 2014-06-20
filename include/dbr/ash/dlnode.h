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
#ifndef DBR_ASH_DLNODE_H
#define DBR_ASH_DLNODE_H

#include <assert.h>
#include <stddef.h> // NULL

/**
 * @addtogroup DlNode
 * @{
 */

/**
 * Doubly-linked node.
 */
struct DbrDlNode {
    struct DbrDlNode* prev;
    struct DbrDlNode* next;
};

#define DBR_DLNODE_INIT(n) { .prev = NULL, .next = NULL }

static inline void
dbr_dlnode_init(struct DbrDlNode* node)
{
    assert(node);
    node->prev = node->next = NULL;
}

static inline void
dbr_dlnode_insert(struct DbrDlNode* prev, struct DbrDlNode* next, struct DbrDlNode* new_node)
{
    assert(prev);
    assert(next);

    prev->next = new_node;
    new_node->prev = prev;

    next->prev = new_node;
    new_node->next = next;
}

static inline void
dbr_dlnode_insert_before(struct DbrDlNode* node, struct DbrDlNode* new_node)
{
    assert(node);
    dbr_dlnode_insert(node->prev, node, new_node);
}

static inline void
dbr_dlnode_insert_after(struct DbrDlNode* node, struct DbrDlNode* new_node)
{
    assert(node);
    dbr_dlnode_insert(node, node->next, new_node);
}

static inline void
dbr_dlnode_remove(struct DbrDlNode* node)
{
    assert(node);
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static inline struct DbrDlNode*
dbr_dlnode_next(struct DbrDlNode* node)
{
    assert(node);
    return node->next;
}

static inline struct DbrDlNode*
dbr_dlnode_prev(struct DbrDlNode* node)
{
    assert(node);
    return node->prev;
}

/** @} */

#endif // DBR_ASH_DLNODE_H
