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
#ifndef ASH_RBNODE_H
#define ASH_RBNODE_H

#include <dbr/node.h>

#include <assert.h>
#include <stddef.h>

#define ASH_RBNODE_INIT(n, k) { .key = k, .left = NULL, .right = NULL, .parent = NULL, .color = 0 }

static inline void
ash_rbnode_init(struct DbrRbNode* node, DbrKey key)
{
    node->key = key;
    node->left = node->right = node->parent = NULL;
    node->color = 0;
}

DBR_API struct DbrRbNode*
ash_rbnode_next(struct DbrRbNode* node);

DBR_API struct DbrRbNode*
ash_rbnode_prev(struct DbrRbNode* node);

#endif // ASH_RBNODE_H
