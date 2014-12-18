/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_DLNODE_H
#define DBR_ASH_DLNODE_H

/**
 * @file
 * Doubly-linked node.
 */

#include <assert.h>
#include <stddef.h> // NULL

/**
 * @addtogroup DlNode
 * @{
 */

/**
 * Doubly-linked node.
 */
struct DbrDlNode {
    struct DbrDlNode* prev;
    struct DbrDlNode* next;
};

#define DBR_DLNODE_INIT(n) { .prev = NULL, .next = NULL }

static inline void
dbr_dlnode_init(struct DbrDlNode* node)
{
    assert(node);
    node->prev = node->next = NULL;
}

static inline void
dbr_dlnode_insert(struct DbrDlNode* prev, struct DbrDlNode* next, struct DbrDlNode* new_node)
{
    assert(prev);
    assert(next);

    prev->next = new_node;
    new_node->prev = prev;

    next->prev = new_node;
    new_node->next = next;
}

static inline void
dbr_dlnode_insert_before(struct DbrDlNode* node, struct DbrDlNode* new_node)
{
    assert(node);
    dbr_dlnode_insert(node->prev, node, new_node);
}

static inline void
dbr_dlnode_insert_after(struct DbrDlNode* node, struct DbrDlNode* new_node)
{
    assert(node);
    dbr_dlnode_insert(node, node->next, new_node);
}

static inline void
dbr_dlnode_remove(struct DbrDlNode* node)
{
    assert(node);
    node->next->prev = node->prev;
    node->prev->next = node->next;
}

static inline struct DbrDlNode*
dbr_dlnode_next(struct DbrDlNode* node)
{
    assert(node);
    return node->next;
}

static inline struct DbrDlNode*
dbr_dlnode_prev(struct DbrDlNode* node)
{
    assert(node);
    return node->prev;
}

/** @} */

#endif // DBR_ASH_DLNODE_H
