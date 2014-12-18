/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_LIST_H
#define SC_ASH_LIST_H

/**
 * @file
 * Doubly-linked list.
 */

#include <sc/ash/defs.h>
#include <sc/ash/dlnode.h>

/**
 * @addtogroup List
 * @{
 */

struct ScList {
    struct ScDlNode end;
};

#define SC_LIST_INIT(l) { .end = { .prev = &l.end, .next = &l.end } }

static inline void
sc_list_init(struct ScList* list)
{
    list->end.prev = &list->end;
    list->end.next = &list->end;
}

static inline void
sc_list_insert_front(struct ScList* list, struct ScDlNode* new_node)
{
    sc_dlnode_insert_before(list->end.next, new_node);
}

static inline void
sc_list_insert_back(struct ScList* list, struct ScDlNode* new_node)
{
    sc_dlnode_insert_after(list->end.prev, new_node);
}

static inline struct ScDlNode*
sc_list_remove_first(struct ScList* list)
{
    assert(list->end.next != &list->end);
    struct ScDlNode* node = list->end.next;
    sc_dlnode_remove(node);
    return node;
}

static inline struct ScDlNode*
sc_list_remove_last(struct ScList* list)
{
    assert(list->end.next != &list->end);
    struct ScDlNode* node = list->end.prev;
    sc_dlnode_remove(node);
    return node;
}

static inline struct ScDlNode*
sc_list_pop(struct ScList* list)
{
    return sc_list_remove_first(list);
}

static inline void
sc_list_push(struct ScList* list, struct ScDlNode* new_node)
{
    sc_list_insert_front(list, new_node);
}

static inline struct ScDlNode*
sc_list_first(const struct ScList* list)
{
    return list->end.next;
}

static inline struct ScDlNode*
sc_list_last(const struct ScList* list)
{
    return list->end.prev;
}

static inline struct ScDlNode*
sc_list_end(const struct ScList* list)
{
    return (struct ScDlNode*)&list->end;
}

static inline ScBool
sc_list_empty(const struct ScList* list)
{
    return list->end.next == &list->end;
}

/** @} */

#endif // SC_ASH_LIST_H
