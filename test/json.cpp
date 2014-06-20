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
#include <dbrpp/json.hpp>

#include <dbr/ash/util.h> // dbr_millis()

using namespace dbr;

TEST_CASE(json_order)
{
    Pool pool(8 * 1024 * 1024);
    auto user = create_wramirez(pool);
    auto group = create_dbra(pool);
    auto contr = create_eurusd(pool);
    auto now = dbr_millis();

    auto in = create_order(pool, 1, *user, *group, *contr, dbr_ymd_to_jd(2014, 3, 14),
                           "apple", DBR_ACTION_BUY, 12345, 10, 0, now);
    in->c.state = DBR_STATE_NEW;

    auto len = dbr_json_order_len(in.get());
    char buf[len + 1];
    const char* end = dbr_json_write_order(buf, in.get());
    check(buf + len == end);
    check(buf[0] == '{');
    check(buf[len - 1] == '}');
}

TEST_CASE(json_trade)
{
    Pool pool(8 * 1024 * 1024);
    auto user = create_wramirez(pool);
    auto group = create_dbra(pool);
    auto contr = create_eurusd(pool);
    auto cpty = create_dbrb(pool);
    auto now = dbr_millis();

    auto in = create_trade(pool, 1, 2, *user, *group, *contr, dbr_ymd_to_jd(2014, 3, 14), "apple",
                           DBR_ACTION_BUY, 12345, 10, 0, 10, 12345, 10, 3, DBR_ROLE_TAKER,
                           *cpty, now);
    in->c.state = DBR_STATE_TRADE;

    auto len = dbr_json_exec_len(in.get());
    char buf[len + 1];
    const char* end = dbr_json_write_exec(buf, in.get());
    check(buf + len == end);
    check(buf[0] == '{');
    check(buf[len - 1] == '}');
}
