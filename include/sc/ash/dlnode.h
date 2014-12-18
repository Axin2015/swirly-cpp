/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_DLNODE_H
#define SC_ASH_DLNODE_H

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
struct ScDlNode {
    struct ScDlNode* prev;
    struct ScDlNode* next;
};

#define SC_DLNODE_INIT(n) { .prev = NULL, .next = NULL }

static inline void
sc_dlnode_init(struct ScDlNode* node)
{
    assert(node);
    node->prev = node->next = NULL;
}

static inline void
sc_dlnode_insert(struct ScDlNode* prev, struct ScDlNode* next, struct ScDlNode* new_node)
{
    assert(prev);
    assert(next);

    prev->next = new_node;
    new_node->prev = prev;

    next->prev = new_node;
    new_node->next = next;
}

static inline void
sc_dlnode_insert_before(struct ScDlNode* node, struct ScDlNode* new_node)
{
    assert(node);
    sc_dlnode_insert(node->prev, node, new_node);
}

static inline void
sc_dlnode_insert_after(struct ScDlNode* node, struct ScDlNode* new_node)
{
    assert(node);
    sc_dlnode_insert(node, node->next, new_node);
}

static inline void
sc_dlnode_remove(struct ScDlNode* node)
{
    assert(node);
    node->next->prev = node->prev;
    node->prev->next = node->next;
}

static inline struct ScDlNode*
sc_dlnode_next(struct ScDlNode* node)
{
    assert(node);
    return node->next;
}

static inline struct ScDlNode*
sc_dlnode_prev(struct ScDlNode* node)
{
    assert(node);
    return node->prev;
}

/** @} */

#endif // SC_ASH_DLNODE_H
