/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "test.hpp"

#include <sc/ash/prioq.h>

#include <stdlib.h> // rand()

TEST_CASE(prioq_push)
{
    struct ScPrioq pq;
    sc_prioq_init(&pq);

    int n = 0;
    for (int i = 1; i <= 4096; ++i) {
        check(sc_prioq_push(&pq, i, rand() % 2048));
        ++n;
    }

    ScKey prev = 0;

    const struct ScElem* elem;
    while ((elem = sc_prioq_top(&pq))) {
        check(elem->key >= prev);
        prev = elem->key;
        sc_prioq_pop(&pq);
        --n;
    }
    check(n == 0);
    sc_prioq_term(&pq);
}

TEST_CASE(prioq_remove)
{
    struct ScPrioq pq;
    sc_prioq_init(&pq);

    int n = 0;
    for (int i = 1; i <= 4096; ++i) {
        check(sc_prioq_push(&pq, i, rand() % 2048));
        ++n;
    }

    for (int i = 1; i <= 1024; ++i)
        if (sc_prioq_remove(&pq, rand() % 2048))
            --n;

    ScKey prev = 0;

    const struct ScElem* elem;
    while ((elem = sc_prioq_top(&pq))) {
        check(elem->key >= prev);
        prev = elem->key;
        sc_prioq_pop(&pq);
        --n;
    }
    check(n == 0);
    sc_prioq_term(&pq);
}

TEST_CASE(prioq_replace)
{
    struct ScPrioq pq;
    sc_prioq_init(&pq);

    int n = 0;
    for (int i = 1; i <= 4096; ++i) {
        check(sc_prioq_push(&pq, i, rand() % 2048));
        ++n;
    }

    for (int i = 1; i <= 1024; ++i)
        sc_prioq_replace(&pq, i, rand() % 2048);

    ScKey prev = 0;

    const struct ScElem* elem;
    while ((elem = sc_prioq_top(&pq))) {
        check(elem->key >= prev);
        prev = elem->key;
        sc_prioq_pop(&pq);
        --n;
    }
    check(n == 0);
    sc_prioq_term(&pq);
}
