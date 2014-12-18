/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_TREE_H
#define SC_ASH_TREE_H

/**
 * @file
 * Red-black tree.
 */

#include <sc/ash/defs.h>
#include <sc/ash/rbnode.h>

/**
 * @addtogroup Tree
 * @{
 */

struct ScTree {
    struct ScRbNode* root;
};

#define SC_TREE_INIT(s) { .root = NULL }
#define SC_TREE_END NULL

static inline void
sc_tree_init(struct ScTree* tree)
{
    tree->root = NULL;
}

/**
 * @return either the newly inserted node or existing node with matching key.
 */

SC_API struct ScRbNode*
sc_tree_insert(struct ScTree* tree, ScKey key, struct ScRbNode* node);

SC_API void
sc_tree_pinsert(struct ScTree* tree, ScKey key, struct ScRbNode* node,
                 struct ScRbNode* parent);

SC_API struct ScRbNode*
sc_tree_remove(struct ScTree* tree, struct ScRbNode* node);

SC_API struct ScRbNode*
sc_tree_find(const struct ScTree* tree, ScKey key);

SC_API struct ScRbNode*
sc_tree_nfind(const struct ScTree* tree, ScKey key);

SC_API struct ScRbNode*
sc_tree_pfind(const struct ScTree* tree, ScKey key);

SC_API struct ScRbNode*
sc_tree_first(const struct ScTree* tree);

SC_API struct ScRbNode*
sc_tree_last(const struct ScTree* tree);

static inline ScBool
sc_tree_empty(const struct ScTree* tree)
{
    return tree->root == NULL;
}

/** @} */

#endif // SC_ASH_TREE_H
