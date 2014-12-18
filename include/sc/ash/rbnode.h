/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_RBNODE_H
#define SC_ASH_RBNODE_H

/**
 * @file
 * Red-black node.
 */

#include <sc/ash/defs.h>

#include <stddef.h> // NULL

/**
 * @addtogroup RbNode
 * @{
 */

/**
 * Red-black node.
 */
struct ScRbNode {
    ScKey key;
    struct ScRbNode* left;
    struct ScRbNode* right;
    struct ScRbNode* parent;
    int color;
};

#define SC_RBNODE_INIT(n) { .key = 0, .left = NULL, .right = NULL, .parent = NULL, .color = 0 }

static inline void
sc_rbnode_init(struct ScRbNode* node)
{
    node->key = 0;
    node->left = node->right = node->parent = NULL;
    node->color = 0;
}

/**
 * Next node from current node.
 */

SC_API struct ScRbNode*
sc_rbnode_next(struct ScRbNode* node);

/**
 * Previous node from current node.
 */

SC_API struct ScRbNode*
sc_rbnode_prev(struct ScRbNode* node);

/** @} */

#endif // SC_ASH_RBNODE_H
