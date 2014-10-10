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
#include "factory.hpp"
#include "mock.hpp"
#include "test.hpp"

#include <dbrpp/elm/pool.hpp>
#include <dbrpp/elm/proto.hpp>

#include <dbr/ash/util.h> // dbr_millis()

using namespace dbr;

TEST_CASE(proto_accnt)
{
    Pool pool(8 * 1024 * 1024);
    auto in = create_dbra(pool);

    auto len = proto_accnt_len(*in);
    char buf[len];
    const char* end = proto_write_accnt(buf, *in);
    check(buf + len == end);

    DbrRec out;
    end = proto_read_accnt(buf, out);
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
    Pool pool(8 * 1024 * 1024);
    auto in = create_eurusd(pool);

    auto len = proto_contr_len(*in);
    char buf[len];
    const char* end = proto_write_contr(buf, *in);
    check(buf + len == end);

    DbrRec out;
    end = proto_read_contr(buf, out);
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

TEST_CASE(proto_perm)
{
    Pool pool(8 * 1024 * 1024);
    DbrIden trader = 5;
    DbrIden giveup = 7;

    auto in = create_perm(pool, trader, giveup);

    auto len = proto_perm_len(*in, false);
    char buf[len];
    const char* end = proto_write_perm(buf, *in, false);
    check(buf + len == end);

    DbrPerm out;
    end = proto_read_perm(buf, out);
    check(buf + len == end);

    check(out.trader.id_only == in->trader.id_only);
    check(out.giveup.id_only == in->giveup.id_only);
}

TEST_CASE(proto_order)
{
    Pool pool(8 * 1024 * 1024);
    DbrIden trader = 5;
    DbrIden giveup = 7;
    DbrIden contr = 11;
    auto now = dbr_millis();

    auto in = create_order(pool, 1, trader, giveup, contr, dbr_ymd_to_jd(2014, 3, 14),
                           "apple", DBR_ACTION_BUY, 12345, 10, 0, now);

    auto len = proto_order_len(*in, false);
    char buf[len];
    const char* end = proto_write_order(buf, *in, false);
    check(buf + len == end);

    DbrOrder out;
    end = proto_read_order(buf, out);
    check(buf + len == end);

    check(out.id == in->id);
    check(out.i.trader.id_only == in->i.trader.id_only);
    check(out.i.giveup.id_only == in->i.giveup.id_only);
    check(out.i.contr.id_only == in->i.contr.id_only);
    check(out.i.settl_day == in->i.settl_day);
    check(sequal(out.i.ref, in->i.ref, DBR_REF_MAX));
    check(out.i.state == in->i.state);
    check(out.i.action == in->i.action);
    check(out.i.ticks == in->i.ticks);
    check(out.i.lots == in->i.lots);
    check(out.i.resd == in->i.resd);
    check(out.i.exec == in->i.exec);
    check(out.i.last_ticks == in->i.last_ticks);
    check(out.i.last_lots == in->i.last_lots);
    check(out.i.min_lots == in->i.min_lots);
    check(out.created == in->created);
    check(out.modified == in->modified);
}

TEST_CASE(proto_trade)
{
    Pool pool(8 * 1024 * 1024);
    DbrIden trader = 5;
    DbrIden giveup = 7;
    DbrIden contr = 11;
    DbrIden cpty = 13;
    auto now = dbr_millis();

    auto in = create_trade(pool, 1, 2, trader, giveup, contr, dbr_ymd_to_jd(2014, 3, 14), "apple",
                           DBR_ACTION_BUY, 12345, 10, 0, 10, 12345, 10, 3, DBR_ROLE_TAKER,
                           cpty, now);

    auto len = proto_exec_len(*in, false);
    char buf[len];
    const char* end = proto_write_exec(buf, *in, false);
    check(buf + len == end);

    DbrExec out;
    end = proto_read_exec(buf, out);
    check(buf + len == end);

    check(out.id == in->id);
    check(out.order == in->order);
    check(out.i.trader.id_only == in->i.trader.id_only);
    check(out.i.giveup.id_only == in->i.giveup.id_only);
    check(out.i.contr.id_only == in->i.contr.id_only);
    check(out.i.settl_day == in->i.settl_day);
    check(sequal(out.i.ref, in->i.ref, DBR_REF_MAX));
    check(out.i.state == in->i.state);
    check(out.i.action == in->i.action);
    check(out.i.ticks == in->i.ticks);
    check(out.i.lots == in->i.lots);
    check(out.i.resd == in->i.resd);
    check(out.i.exec == in->i.exec);
    check(out.i.last_ticks == in->i.last_ticks);
    check(out.i.last_lots == in->i.last_lots);
    check(out.i.min_lots == in->i.min_lots);
    check(out.match == in->match);
    check(out.role == in->role);
    check(out.cpty.id_only == in->cpty.id_only);
    check(out.created == in->created);
}
