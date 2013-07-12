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
#ifndef ASH_TREE_H
#define ASH_TREE_H

#include <dbr/bool.h>
#include <dbr/node.h>

#include <stddef.h> // NULL

struct AshTree {
    struct DbrRbNode* root;
};

#define ASH_TREE_INIT(s) { .root = NULL }

static inline void
ash_tree_init(struct AshTree* tree)
{
    tree->root = NULL;
}

DBR_API struct DbrRbNode*
ash_tree_insert(struct AshTree* tree, struct DbrRbNode* node);

DBR_API void
ash_tree_pinsert(struct AshTree* tree, struct DbrRbNode* node, struct DbrRbNode* parent);

DBR_API struct DbrRbNode*
ash_tree_remove(struct AshTree* tree, struct DbrRbNode* node);

DBR_API struct DbrRbNode*
ash_tree_find(const struct AshTree* tree, DbrKey key);

DBR_API struct DbrRbNode*
ash_tree_nfind(const struct AshTree* tree, DbrKey key);

DBR_API struct DbrRbNode*
ash_tree_pfind(const struct AshTree* tree, DbrKey key);

DBR_API struct DbrRbNode*
ash_tree_first(const struct AshTree* tree);

DBR_API struct DbrRbNode*
ash_tree_last(const struct AshTree* tree);

static inline struct DbrRbNode*
ash_tree_end(const struct AshTree* tree)
{
    return NULL;
}

static inline DbrBool
ash_tree_empty(const struct AshTree* tree)
{
    return tree->root == NULL;
}

#endif // ASH_TREE_H
