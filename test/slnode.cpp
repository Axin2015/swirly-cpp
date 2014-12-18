/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "test.hpp"

#include <sc/ash/list.h>
#include <sc/ash/queue.h>
#include <sc/ash/stack.h>

struct SlElem {
    int id;
    ScSlNode node;
};

TEST_CASE(slnode_list)
{
    ScList l = SC_LIST_INIT(l);

    check(sc_list_empty(&l));
    check(sc_list_first(&l) == sc_list_end(&l));
}

TEST_CASE(slnode_queue)
{
    ScQueue q = SC_QUEUE_INIT(q);

    check(sc_queue_empty(&q));
    check(sc_queue_first(&q) == SC_QUEUE_END);

    SlElem e1 = { .id = 1, .node = { .next = NULL } };
    SlElem e2 = { .id = 2, .node = { .next = NULL } };
    SlElem e3 = { .id = 3, .node = { .next = NULL } };

    sc_queue_insert_front(&q, &e1.node);
    sc_queue_insert_front(&q, &e2.node);
    sc_queue_insert_front(&q, &e3.node);

    check(!sc_queue_empty(&q));
    check(sc_queue_first(&q) == &e3.node);
    check(sc_queue_first(&q)->next == &e2.node);
    check(sc_queue_first(&q)->next->next == &e1.node);
    check(sc_queue_first(&q)->next->next->next == SC_QUEUE_END);

    check(sc_queue_remove_first(&q) == &e3.node);
    check(!sc_queue_empty(&q));
    check(sc_queue_first(&q) == &e2.node);
    check(sc_queue_first(&q)->next == &e1.node);
    check(sc_queue_first(&q)->next->next == SC_QUEUE_END);

    check(sc_queue_remove_first(&q) == &e2.node);
    check(!sc_queue_empty(&q));
    check(sc_queue_first(&q) == &e1.node);
    check(sc_queue_first(&q)->next == SC_QUEUE_END);

    check(sc_queue_remove_first(&q) == &e1.node);
    check(sc_queue_empty(&q));
    check(sc_queue_first(&q) == SC_QUEUE_END);

    sc_queue_insert_front(&q, &e1.node);
    sc_queue_insert_after(&q, &e1.node, &e2.node);
    sc_queue_insert_after(&q, &e2.node, &e3.node);

    check(!sc_queue_empty(&q));
    check(sc_queue_first(&q) == &e1.node);
    check(sc_queue_first(&q)->next == &e2.node);
    check(sc_queue_first(&q)->next->next == &e3.node);
    check(sc_queue_first(&q)->next->next->next == SC_QUEUE_END);
}

TEST_CASE(slnode_stack)
{
    ScStack s = SC_STACK_INIT(s);

    check(sc_stack_empty(&s));
    check(sc_stack_first(&s) == SC_STACK_END);
}
