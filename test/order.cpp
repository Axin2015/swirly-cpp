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
#include "factory.hpp"
#include "test.hpp"

#include <dbrpp/order.hpp>
#include <dbrpp/pool.hpp>

using namespace dbr;

TEST_CASE(order_proto)
{
    Pool pool;
    DbrIden trader = 5;
    DbrIden accnt = 7;
    DbrIden contr = 11;

    auto in = create_order(pool, 1, trader, accnt, contr, 20130827,
                           "apple", DBR_BUY, 12345, 10, 0, 0);

    auto len = order_len(*in);
    char buf[len];
    const char* end = write_order(buf, *in);
    check(buf + len == end);

    DbrOrder out;
    end = read_order(buf, out);
    check(buf + len == end);

    check(out.id == in->id);
    check(out.rev == in->rev);
    check(out.status == in->status);
    check(out.trader.id == in->trader.id);
    check(out.accnt.id == in->accnt.id);
    check(out.contr.id == in->contr.id);
    check(out.settl_date == in->settl_date);
    check(sequal(out.ref, in->ref, DBR_REF_MAX));
    check(out.action == in->action);
    check(out.ticks == in->ticks);
    check(out.resd == in->resd);
    check(out.exec == in->exec);
    check(out.lots == in->lots);
    check(out.min == in->min);
    check(out.flags == in->flags);
    check(out.created == in->created);
    check(out.modified == in->modified);
}
