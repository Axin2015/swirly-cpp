/*
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
#include "test.h"

#include <ash/list.h>
#include <ash/queue.h>
#include <ash/stack.h>
#include <ash/slnode.h>

struct SlElem {
    int id;
    struct DbrSlNode node;
};

static inline struct SlElem*
dbr_elem_from_node(struct DbrSlNode* node)
{
    return dbr_implof(struct SlElem, node, node);
}

void
ash_list(void)
{
    struct AshList l = ASH_LIST_INIT(l);

    check(ash_list_empty(&l));
    check(ash_list_first(&l) == ash_list_end(&l));
}

void
ash_queue(void)
{
    struct AshQueue q = ASH_QUEUE_INIT(q);

    check(ash_queue_empty(&q));
    check(ash_queue_first(&q) == ash_queue_end(&q));

    struct SlElem e1 = { .id = 1, .node = { .next = NULL } };
    struct SlElem e2 = { .id = 2, .node = { .next = NULL } };
    struct SlElem e3 = { .id = 3, .node = { .next = NULL } };

    ash_queue_insert_front(&q, &e1.node);
    ash_queue_insert_front(&q, &e2.node);
    ash_queue_insert_front(&q, &e3.node);

    check(!ash_queue_empty(&q));
    check(ash_queue_first(&q) == &e3.node);
    check(ash_queue_first(&q)->next == &e2.node);
    check(ash_queue_first(&q)->next->next == &e1.node);
    check(ash_queue_first(&q)->next->next->next == ash_queue_end(&q));

    check(ash_queue_remove_first(&q) == &e3.node);
    check(!ash_queue_empty(&q));
    check(ash_queue_first(&q) == &e2.node);
    check(ash_queue_first(&q)->next == &e1.node);
    check(ash_queue_first(&q)->next->next == ash_queue_end(&q));

    check(ash_queue_remove_first(&q) == &e2.node);
    check(!ash_queue_empty(&q));
    check(ash_queue_first(&q) == &e1.node);
    check(ash_queue_first(&q)->next == ash_queue_end(&q));

    check(ash_queue_remove_first(&q) == &e1.node);
    check(ash_queue_empty(&q));
    check(ash_queue_first(&q) == ash_queue_end(&q));

    ash_queue_insert_front(&q, &e1.node);
    ash_queue_insert_after(&q, &e1.node, &e2.node);
    ash_queue_insert_after(&q, &e2.node, &e3.node);

    check(!ash_queue_empty(&q));
    check(ash_queue_first(&q) == &e1.node);
    check(ash_queue_first(&q)->next == &e2.node);
    check(ash_queue_first(&q)->next->next == &e3.node);
    check(ash_queue_first(&q)->next->next->next == ash_queue_end(&q));
}

void
ash_stack(void)
{
    struct AshStack s = ASH_STACK_INIT(s);

    check(ash_stack_empty(&s));
    check(ash_stack_first(&s) == ash_stack_end(&s));
}
