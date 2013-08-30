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
#include "test.hpp"

#include <dbrpp/order.hpp>
#include <dbrpp/pool.hpp>

#include <dbr/util.h>

#include <memory>

using namespace dbr;

std::shared_ptr<DbrRec>
make_contr(Pool& pool, DbrIden id, const char* mnem)
{
    auto deleter = [&pool](DbrRec* rec) {
        pool.free_rec(rec);
    };
    std::shared_ptr<DbrRec> contr(pool.alloc_rec(), deleter);

    contr->id = id;
    strncpy(contr->mnem, mnem, DBR_MNEM_MAX);
    return contr;
}

std::shared_ptr<DbrRec>
make_trader(Pool& pool, DbrIden id, const char* mnem)
{
    auto deleter = [&pool](DbrRec* rec) {
        pool.free_rec(rec);
    };
    std::shared_ptr<DbrRec> trader(pool.alloc_rec(), deleter);

    trader->id = id;
    strncpy(trader->mnem, mnem, DBR_MNEM_MAX);
    return trader;
}

std::shared_ptr<DbrRec>
make_accnt(Pool& pool, DbrIden id, const char* mnem)
{
    auto deleter = [&pool](DbrRec* rec) {
        pool.free_rec(rec);
    };
    std::shared_ptr<DbrRec> accnt(pool.alloc_rec(), deleter);

    accnt->id = id;
    strncpy(accnt->mnem, mnem, DBR_MNEM_MAX);
    return accnt;
}

std::shared_ptr<DbrOrder>
make_order(Pool& pool, DbrIden id, DbrRec& trader, DbrRec& accnt, DbrRec& contr,
           DbrDate settl_date, const char* ref, int action, DbrTicks ticks, DbrLots lots,
           DbrLots min, DbrFlags flags)
{
    auto deleter = [&pool](DbrOrder* order) {
        pool.free_order(order);
    };
    std::shared_ptr<DbrOrder> order(pool.alloc_order(id), deleter);

    order->id = id;
    order->level = NULL;
    order->rev = 1;
    order->status = DBR_NEW;
    order->trader.rec = &trader;
    order->accnt.rec = &accnt;
    order->contr.rec = &contr;
    order->settl_date = settl_date;
    if (ref)
        strncpy(order->ref, ref, DBR_REF_MAX);
    else
        order->ref[0] = '\0';

    order->action = action;
    order->ticks = ticks;
    order->resd = lots;
    order->exec = 0;
    order->lots = lots;
    order->min = min;
    order->flags = flags;
    const DbrMillis now = dbr_millis();
    order->created = now;
    order->modified = now;

    return order;
}

TEST_CASE(order_test)
{
    Pool pool;
    auto contr = make_contr(pool, 1, "EURUSD");
    auto trader = make_contr(pool, 1, "WRAMIREZ");
    auto accnt = make_contr(pool, 1, "DBRA");

    auto apple = make_order(pool, 1, *trader, *accnt, *contr, 20130827,
                            "apple", DBR_BUY, 12345, 10, 0, 0);
    auto orange = make_order(pool, 1, *trader, *accnt, *contr, 20130827,
                             "orange", DBR_BUY, 12345, 10, 0, 0);
}
