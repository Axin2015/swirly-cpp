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
#include "mock.hpp"
#include "test.hpp"

#include <dbrpp/pool.hpp>

#include <dbr/proto.h>

using namespace dbr;

TEST_CASE(proto_trader)
{
    Pool pool;
    auto in = create_wramirez(pool);

    auto len = trader_len(*in);
    char buf[len];
    const char* end = write_trader(buf, *in);
    check(buf + len == end);

    DbrRec out;
    end = read_trader(buf, out);
    check(buf + len == end);

    check(out.type == in->type);
    check(out.id == in->id);
    check(sequal(out.mnem, in->mnem, DBR_MNEM_MAX));
    check(sequal(out.display, in->display, DBR_DISPLAY_MAX));

    // Body.
    check(sequal(out.trader.email, in->trader.email, DBR_EMAIL_MAX));
}

TEST_CASE(proto_accnt)
{
    Pool pool;
    auto in = create_dbra(pool);

    auto len = accnt_len(*in);
    char buf[len];
    const char* end = write_accnt(buf, *in);
    check(buf + len == end);

    DbrRec out;
    end = read_accnt(buf, out);
    check(buf + len == end);

    check(out.type == in->type);
    check(out.id == in->id);
    check(sequal(out.mnem, in->mnem, DBR_MNEM_MAX));
    check(sequal(out.display, in->display, DBR_DISPLAY_MAX));

    // Body.
    check(sequal(out.accnt.email, in->accnt.email, DBR_EMAIL_MAX));
}

TEST_CASE(proto_contr)
{
    Pool pool;
    auto in = create_eurusd(pool);

    auto len = contr_len(*in);
    char buf[len];
    const char* end = write_contr(buf, *in);
    check(buf + len == end);

    DbrRec out;
    end = read_contr(buf, out);
    check(buf + len == end);

    check(out.type == in->type);
    check(out.id == in->id);
    check(sequal(out.mnem, in->mnem, DBR_MNEM_MAX));
    check(sequal(out.display, in->display, DBR_DISPLAY_MAX));

    // Body.
    check(sequal(out.contr.asset_type, in->contr.asset_type, DBR_MNEM_MAX));
    check(sequal(out.contr.asset, in->contr.asset, DBR_MNEM_MAX));
    check(sequal(out.contr.ccy, in->contr.ccy, DBR_MNEM_MAX));
    check(out.contr.tick_numer == in->contr.tick_numer);
    check(out.contr.tick_denom == in->contr.tick_denom);
    check(fequal(out.contr.price_inc, in->contr.price_inc));
    check(out.contr.lot_numer == in->contr.lot_numer);
    check(out.contr.lot_denom == in->contr.lot_denom);
    check(fequal(out.contr.qty_inc, in->contr.qty_inc));
    check(fequal(out.contr.price_dp, in->contr.price_dp));
    check(out.contr.pip_dp == in->contr.pip_dp);
    check(fequal(out.contr.qty_dp, in->contr.qty_dp));
    check(out.contr.min_lots == in->contr.min_lots);
    check(out.contr.max_lots == in->contr.max_lots);
}

TEST_CASE(proto_order)
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
