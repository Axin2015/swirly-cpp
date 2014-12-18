/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_TREE_H
#define DBR_ASH_TREE_H

/**
 * @file
 * Red-black tree.
 */

#include <dbr/ash/defs.h>
#include <dbr/ash/rbnode.h>

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

/**
 * @return either the newly inserted node or existing node with matching key.
 */

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

#endif // DBR_ASH_TREE_H
