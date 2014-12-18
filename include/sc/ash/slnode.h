/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_SLNODE_H
#define SC_ASH_SLNODE_H

/**
 * @file
 * Singly-linked node.
 */

#include <assert.h>
#include <stddef.h> // NULL

/**
 * @addtogroup SlNode
 * @{
 */

/**
 * Singly-linked node.
 */
struct ScSlNode {
    struct ScSlNode* next;
};

#define SC_SLNODE_INIT(n) { .next = NULL }

static inline void
sc_slnode_init(struct ScSlNode* node)
{
    assert(node);
    node->next = NULL;
}

static inline struct ScSlNode*
sc_slnode_next(struct ScSlNode* node)
{
    assert(node);
    return node->next;
}

/** @} */

#endif // SC_ASH_SLNODE_H
