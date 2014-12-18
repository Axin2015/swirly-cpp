/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_LIST_H
#define DBR_ASH_LIST_H

/**
 * @file
 * Doubly-linked list.
 */

#include <dbr/ash/defs.h>
#include <dbr/ash/dlnode.h>

/**
 * @addtogroup List
 * @{
 */

struct DbrList {
    struct DbrDlNode end;
};

#define DBR_LIST_INIT(l) { .end = { .prev = &l.end, .next = &l.end } }

static inline void
dbr_list_init(struct DbrList* list)
{
    list->end.prev = &list->end;
    list->end.next = &list->end;
}

static inline void
dbr_list_insert_front(struct DbrList* list, struct DbrDlNode* new_node)
{
    dbr_dlnode_insert_before(list->end.next, new_node);
}

static inline void
dbr_list_insert_back(struct DbrList* list, struct DbrDlNode* new_node)
{
    dbr_dlnode_insert_after(list->end.prev, new_node);
}

static inline struct DbrDlNode*
dbr_list_remove_first(struct DbrList* list)
{
    assert(list->end.next != &list->end);
    struct DbrDlNode* node = list->end.next;
    dbr_dlnode_remove(node);
    return node;
}

static inline struct DbrDlNode*
dbr_list_remove_last(struct DbrList* list)
{
    assert(list->end.next != &list->end);
    struct DbrDlNode* node = list->end.prev;
    dbr_dlnode_remove(node);
    return node;
}

static inline struct DbrDlNode*
dbr_list_pop(struct DbrList* list)
{
    return dbr_list_remove_first(list);
}

static inline void
dbr_list_push(struct DbrList* list, struct DbrDlNode* new_node)
{
    dbr_list_insert_front(list, new_node);
}

static inline struct DbrDlNode*
dbr_list_first(const struct DbrList* list)
{
    return list->end.next;
}

static inline struct DbrDlNode*
dbr_list_last(const struct DbrList* list)
{
    return list->end.prev;
}

static inline struct DbrDlNode*
dbr_list_end(const struct DbrList* list)
{
    return (struct DbrDlNode*)&list->end;
}

static inline DbrBool
dbr_list_empty(const struct DbrList* list)
{
    return list->end.next == &list->end;
}

/** @} */

#endif // DBR_ASH_LIST_H
