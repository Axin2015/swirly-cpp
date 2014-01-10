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

#include <dbr/prioq.h>

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

    long prev = 0;

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

    long prev = 0;

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

    long prev = 0;

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
