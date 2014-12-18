/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "test.hpp"

#include <dbr/ash/prioq.h>

#include <stdlib.h> // rand()

TEST_CASE(prioq_push)
{
    struct DbrPrioq pq;
    dbr_prioq_init(&pq);

    int n = 0;
    for (int i = 1; i <= 4096; ++i) {
        check(dbr_prioq_push(&pq, i, rand() % 2048));
        ++n;
    }

    DbrKey prev = 0;

    const struct DbrElem* elem;
    while ((elem = dbr_prioq_top(&pq))) {
        check(elem->key >= prev);
        prev = elem->key;
        dbr_prioq_pop(&pq);
        --n;
    }
    check(n == 0);
    dbr_prioq_term(&pq);
}

TEST_CASE(prioq_remove)
{
    struct DbrPrioq pq;
    dbr_prioq_init(&pq);

    int n = 0;
    for (int i = 1; i <= 4096; ++i) {
        check(dbr_prioq_push(&pq, i, rand() % 2048));
        ++n;
    }

    for (int i = 1; i <= 1024; ++i)
        if (dbr_prioq_remove(&pq, rand() % 2048))
            --n;

    DbrKey prev = 0;

    const struct DbrElem* elem;
    while ((elem = dbr_prioq_top(&pq))) {
        check(elem->key >= prev);
        prev = elem->key;
        dbr_prioq_pop(&pq);
        --n;
    }
    check(n == 0);
    dbr_prioq_term(&pq);
}

TEST_CASE(prioq_replace)
{
    struct DbrPrioq pq;
    dbr_prioq_init(&pq);

    int n = 0;
    for (int i = 1; i <= 4096; ++i) {
        check(dbr_prioq_push(&pq, i, rand() % 2048));
        ++n;
    }

    for (int i = 1; i <= 1024; ++i)
        dbr_prioq_replace(&pq, i, rand() % 2048);

    DbrKey prev = 0;

    const struct DbrElem* elem;
    while ((elem = dbr_prioq_top(&pq))) {
        check(elem->key >= prev);
        prev = elem->key;
        dbr_prioq_pop(&pq);
        --n;
    }
    check(n == 0);
    dbr_prioq_term(&pq);
}
