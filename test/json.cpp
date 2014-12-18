/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "factory.hpp"
#include "mock.hpp"
#include "test.hpp"

#include <dbrpp/elm/json.hpp>
#include <dbrpp/elm/pool.hpp>

#include <dbr/ash/util.h> // dbr_millis()

using namespace dbr;

TEST_CASE(json_order)
{
    Pool pool(8 * 1024 * 1024);
    auto trader = create_wramirez(pool);
    auto giveup = create_dbra(pool);
    auto contr = create_eurusd(pool);
    auto now = dbr_millis();

    auto in = create_order(pool, 1, *trader, *giveup, *contr, dbr_ymd_to_jd(2014, 3, 14),
                           "apple", DBR_ACTION_BUY, 12345, 10, 0, now);
    in->i.state = DBR_STATE_NEW;

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
    auto trader = create_wramirez(pool);
    auto giveup = create_dbra(pool);
    auto contr = create_eurusd(pool);
    auto cpty = create_dbrb(pool);
    auto now = dbr_millis();

    auto in = create_trade(pool, 1, 2, *trader, *giveup, *contr, dbr_ymd_to_jd(2014, 3, 14), "apple",
                           DBR_ACTION_BUY, 12345, 10, 0, 10, 12345, 10, 3, DBR_ROLE_TAKER,
                           *cpty, now);
    in->i.state = DBR_STATE_TRADE;

    auto len = dbr_json_exec_len(in.get());
    char buf[len + 1];
    const char* end = dbr_json_write_exec(buf, in.get());
    check(buf + len == end);
    check(buf[0] == '{');
    check(buf[len - 1] == '}');
}
