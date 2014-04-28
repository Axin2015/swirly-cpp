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

#include <dbrpp/pool.hpp>
#include <dbrpp/proto.hpp>

#include <dbr/util.h> // dbr_millis()

using namespace dbr;

TEST_CASE(proto_accnt)
{
    Pool pool(8 * 1024 * 1024);
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
    Pool pool(8 * 1024 * 1024);
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

TEST_CASE(proto_memb)
{
    Pool pool(8 * 1024 * 1024);
    DbrIden user = 5;
    DbrIden group = 7;

    auto in = create_memb(pool, user, group);

    auto len = memb_len(*in, false);
    char buf[len];
    const char* end = write_memb(buf, *in, false);
    check(buf + len == end);

    DbrMemb out;
    end = read_memb(buf, out);
    check(buf + len == end);

    check(out.user.id_only == in->user.id_only);
    check(out.group.id_only == in->group.id_only);
}

TEST_CASE(proto_order)
{
    Pool pool(8 * 1024 * 1024);
    DbrIden user = 5;
    DbrIden group = 7;
    DbrIden contr = 11;
    auto now = dbr_millis();

    auto in = create_order(pool, 1, user, group, contr, dbr_ymd_to_jd(2014, 3, 14),
                           "apple", DBR_ACTION_BUY, 12345, 10, 0, now);

    auto len = order_len(*in, false);
    char buf[len];
    const char* end = write_order(buf, *in, false);
    check(buf + len == end);

    DbrOrder out;
    end = read_order(buf, out);
    check(buf + len == end);

    check(out.id == in->id);
    check(out.c.user.id_only == in->c.user.id_only);
    check(out.c.group.id_only == in->c.group.id_only);
    check(out.c.contr.id_only == in->c.contr.id_only);
    check(out.c.settl_day == in->c.settl_day);
    check(sequal(out.c.ref, in->c.ref, DBR_REF_MAX));
    check(out.c.state == in->c.state);
    check(out.c.action == in->c.action);
    check(out.c.ticks == in->c.ticks);
    check(out.c.lots == in->c.lots);
    check(out.c.resd == in->c.resd);
    check(out.c.exec == in->c.exec);
    check(out.c.last_ticks == in->c.last_ticks);
    check(out.c.last_lots == in->c.last_lots);
    check(out.c.min_lots == in->c.min_lots);
    check(out.created == in->created);
    check(out.modified == in->modified);
}

TEST_CASE(proto_trade)
{
    Pool pool(8 * 1024 * 1024);
    DbrIden user = 5;
    DbrIden group = 7;
    DbrIden contr = 11;
    DbrIden cpty = 13;
    auto now = dbr_millis();

    auto in = create_trade(pool, 1, 2, user, group, contr, dbr_ymd_to_jd(2014, 3, 14), "apple",
                           DBR_ACTION_BUY, 12345, 10, 0, 10, 12345, 10, 3, DBR_ROLE_TAKER,
                           cpty, now);

    auto len = exec_len(*in, false);
    char buf[len];
    const char* end = write_exec(buf, *in, false);
    check(buf + len == end);

    DbrExec out;
    end = read_exec(buf, out);
    check(buf + len == end);

    check(out.id == in->id);
    check(out.order == in->order);
    check(out.c.user.id_only == in->c.user.id_only);
    check(out.c.group.id_only == in->c.group.id_only);
    check(out.c.contr.id_only == in->c.contr.id_only);
    check(out.c.settl_day == in->c.settl_day);
    check(sequal(out.c.ref, in->c.ref, DBR_REF_MAX));
    check(out.c.state == in->c.state);
    check(out.c.action == in->c.action);
    check(out.c.ticks == in->c.ticks);
    check(out.c.lots == in->c.lots);
    check(out.c.resd == in->c.resd);
    check(out.c.exec == in->c.exec);
    check(out.c.last_ticks == in->c.last_ticks);
    check(out.c.last_lots == in->c.last_lots);
    check(out.c.min_lots == in->c.min_lots);
    check(out.match == in->match);
    check(out.role == in->role);
    check(out.cpty.id_only == in->cpty.id_only);
    check(out.created == in->created);
}
