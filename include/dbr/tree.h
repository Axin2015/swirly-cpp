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
#ifndef DBR_TREE_H
#define DBR_TREE_H

#include <dbr/defs.h>
#include <dbr/rbnode.h>

/**
 * @addtogroup Tree
 * @{
 */

struct DbrTree {
    struct DbrRbNode* root;
};

#define DBR_TREE_INIT(s) { .root = NULL }
#define DBR_TREE_END NULL

static inline void
dbr_tree_init(struct DbrTree* tree)
{
    tree->root = NULL;
}

DBR_API struct DbrRbNode*
dbr_tree_insert(struct DbrTree* tree, DbrKey key, struct DbrRbNode* node);

DBR_API void
dbr_tree_pinsert(struct DbrTree* tree, DbrKey key, struct DbrRbNode* node,
                 struct DbrRbNode* parent);

DBR_API struct DbrRbNode*
dbr_tree_remove(struct DbrTree* tree, struct DbrRbNode* node);

DBR_API struct DbrRbNode*
dbr_tree_find(const struct DbrTree* tree, DbrKey key);

DBR_API struct DbrRbNode*
dbr_tree_nfind(const struct DbrTree* tree, DbrKey key);

DBR_API struct DbrRbNode*
dbr_tree_pfind(const struct DbrTree* tree, DbrKey key);

DBR_API struct DbrRbNode*
dbr_tree_first(const struct DbrTree* tree);

DBR_API struct DbrRbNode*
dbr_tree_last(const struct DbrTree* tree);

static inline DbrBool
dbr_tree_empty(const struct DbrTree* tree)
{
    return tree->root == NULL;
}

/** @} */

#endif // DBR_TREE_H
