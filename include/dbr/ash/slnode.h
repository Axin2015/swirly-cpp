/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_SLNODE_H
#define DBR_ASH_SLNODE_H

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
struct DbrSlNode {
    struct DbrSlNode* next;
};

#define DBR_SLNODE_INIT(n) { .next = NULL }

static inline void
dbr_slnode_init(struct DbrSlNode* node)
{
    assert(node);
    node->next = NULL;
}

static inline struct DbrSlNode*
dbr_slnode_next(struct DbrSlNode* node)
{
    assert(node);
    return node->next;
}

/** @} */

#endif // DBR_ASH_SLNODE_H
