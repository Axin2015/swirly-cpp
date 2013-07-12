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
#ifndef DBR_RBNODE_H
#define DBR_RBNODE_H

#include <dbr/defs.h>

#include <stddef.h>

/**
 * @addtogroup Types
 * @{
 */

typedef long DbrKey;

/** @} */

/**
 * @addtogroup Node
 * @{
 */

/**
 * @brief Red-black node.
 */
struct DbrRbNode {
    DbrKey key;
    struct DbrRbNode* left;
    struct DbrRbNode* right;
    struct DbrRbNode* parent;
    int color;
};

#define DBR_RBNODE_INIT(n, k) { .key = k, .left = NULL, .right = NULL, .parent = NULL, .color = 0 }

static inline void
dbr_rbnode_init(struct DbrRbNode* node, DbrKey key)
{
    node->key = key;
    node->left = node->right = node->parent = NULL;
    node->color = 0;
}

/**
 * @brief Next node from current node.
 */

DBR_API struct DbrRbNode*
dbr_rbnode_next(struct DbrRbNode* node);

/**
 * @brief Previous node from current node.
 */

DBR_API struct DbrRbNode*
dbr_rbnode_prev(struct DbrRbNode* node);

/** @} */

#endif // DBR_RBNODE_H
