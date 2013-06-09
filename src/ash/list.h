/**
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#ifndef ASH_LIST_H
#define ASH_LIST_H

#include <ash/dlnode.h>

#include <dbr/bool.h>

struct AshList {
    struct DbrDlNode end;
};

#define ASH_LIST_INIT(l) { .end = { .prev = &l.end, .next = &l.end } }

static inline void
ash_list_init(struct AshList* list)
{
    list->end.prev = &list->end;
    list->end.next = &list->end;
}

static inline void
ash_list_insert_front(struct AshList* list, struct DbrDlNode* new_node)
{
    ash_dlnode_insert_before(list->end.next, new_node);
}

static inline void
ash_list_insert_back(struct AshList* list, struct DbrDlNode* new_node)
{
    ash_dlnode_insert_after(list->end.prev, new_node);
}

static inline struct DbrDlNode*
ash_list_remove_first(struct AshList* list)
{
    struct DbrDlNode* node = list->end.next;
    ash_dlnode_remove(node);
    return node;
}

static inline struct DbrDlNode*
ash_list_remove_last(struct AshList* list)
{
    struct DbrDlNode* node = list->end.prev;
    ash_dlnode_remove(node);
    return node;
}

static inline struct DbrDlNode*
ash_list_pop(struct AshList* list)
{
    return ash_list_remove_first(list);
}

static inline void
ash_list_push(struct AshList* list, struct DbrDlNode* new_node)
{
    ash_list_insert_front(list, new_node);
}

static inline struct DbrDlNode*
ash_list_first(const struct AshList* list)
{
    return list->end.next;
}

static inline struct DbrDlNode*
ash_list_last(const struct AshList* list)
{
    return list->end.prev;
}

static inline struct DbrDlNode*
ash_list_end(const struct AshList* list)
{
    return (struct DbrDlNode*)&list->end;
}

static inline DbrBool
ash_list_empty(const struct AshList* list)
{
    return list->end.next == &list->end;
}

#endif // ASH_LIST_H
