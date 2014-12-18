/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_RBNODE_H
#define DBR_ASH_RBNODE_H

/**
 * @file
 * Red-black node.
 */

#include <dbr/ash/defs.h>

#include <stddef.h> // NULL

/**
 * @addtogroup RbNode
 * @{
 */

/**
 * Red-black node.
 */
struct DbrRbNode {
    DbrKey key;
    struct DbrRbNode* left;
    struct DbrRbNode* right;
    struct DbrRbNode* parent;
    int color;
};

#define DBR_RBNODE_INIT(n) { .key = 0, .left = NULL, .right = NULL, .parent = NULL, .color = 0 }

static inline void
dbr_rbnode_init(struct DbrRbNode* node)
{
    node->key = 0;
    node->left = node->right = node->parent = NULL;
    node->color = 0;
}

/**
 * Next node from current node.
 */

DBR_API struct DbrRbNode*
dbr_rbnode_next(struct DbrRbNode* node);

/**
 * Previous node from current node.
 */

DBR_API struct DbrRbNode*
dbr_rbnode_prev(struct DbrRbNode* node);

/** @} */

#endif // DBR_ASH_RBNODE_H
