/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#include "test.hpp"

#include <dbr/ash/list.h>
#include <dbr/ash/queue.h>
#include <dbr/ash/stack.h>

struct SlElem {
    int id;
    DbrSlNode node;
};

TEST_CASE(slnode_list)
{
    DbrList l = DBR_LIST_INIT(l);

    check(dbr_list_empty(&l));
    check(dbr_list_first(&l) == dbr_list_end(&l));
}

TEST_CASE(slnode_queue)
{
    DbrQueue q = DBR_QUEUE_INIT(q);

    check(dbr_queue_empty(&q));
    check(dbr_queue_first(&q) == DBR_QUEUE_END);

    SlElem e1 = { .id = 1, .node = { .next = NULL } };
    SlElem e2 = { .id = 2, .node = { .next = NULL } };
    SlElem e3 = { .id = 3, .node = { .next = NULL } };

    dbr_queue_insert_front(&q, &e1.node);
    dbr_queue_insert_front(&q, &e2.node);
    dbr_queue_insert_front(&q, &e3.node);

    check(!dbr_queue_empty(&q));
    check(dbr_queue_first(&q) == &e3.node);
    check(dbr_queue_first(&q)->next == &e2.node);
    check(dbr_queue_first(&q)->next->next == &e1.node);
    check(dbr_queue_first(&q)->next->next->next == DBR_QUEUE_END);

    check(dbr_queue_remove_first(&q) == &e3.node);
    check(!dbr_queue_empty(&q));
    check(dbr_queue_first(&q) == &e2.node);
    check(dbr_queue_first(&q)->next == &e1.node);
    check(dbr_queue_first(&q)->next->next == DBR_QUEUE_END);

    check(dbr_queue_remove_first(&q) == &e2.node);
    check(!dbr_queue_empty(&q));
    check(dbr_queue_first(&q) == &e1.node);
    check(dbr_queue_first(&q)->next == DBR_QUEUE_END);

    check(dbr_queue_remove_first(&q) == &e1.node);
    check(dbr_queue_empty(&q));
    check(dbr_queue_first(&q) == DBR_QUEUE_END);

    dbr_queue_insert_front(&q, &e1.node);
    dbr_queue_insert_after(&q, &e1.node, &e2.node);
    dbr_queue_insert_after(&q, &e2.node, &e3.node);

    check(!dbr_queue_empty(&q));
    check(dbr_queue_first(&q) == &e1.node);
    check(dbr_queue_first(&q)->next == &e2.node);
    check(dbr_queue_first(&q)->next->next == &e3.node);
    check(dbr_queue_first(&q)->next->next->next == DBR_QUEUE_END);
}

TEST_CASE(slnode_stack)
{
    DbrStack s = DBR_STACK_INIT(s);

    check(dbr_stack_empty(&s));
    check(dbr_stack_first(&s) == DBR_STACK_END);
}
