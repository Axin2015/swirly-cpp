/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "factory.hpp"
#include "mock.hpp"
#include "test.hpp"

#include <scpp/elm/json.hpp>
#include <scpp/elm/pool.hpp>

#include <sc/ash/util.h> // sc_millis()

using namespace sc;

TEST_CASE(json_order)
{
    Pool pool(8 * 1024 * 1024);
    auto trader = create_wramirez(pool);
    auto giveup = create_sca(pool);
    auto contr = create_eurusd(pool);
    auto now = sc_millis();

    auto in = create_order(pool, 1, *trader, *giveup, *contr, sc_ymd_to_jd(2014, 3, 14),
                           "apple", SC_ACTION_BUY, 12345, 10, 0, now);
    in->i.state = SC_STATE_NEW;

    auto len = sc_json_order_len(in.get());
    char buf[len + 1];
    const char* end = sc_json_write_order(buf, in.get());
    check(buf + len == end);
    check(buf[0] == '{');
    check(buf[len - 1] == '}');
}

TEST_CASE(json_trade)
{
    Pool pool(8 * 1024 * 1024);
    auto trader = create_wramirez(pool);
    auto giveup = create_sca(pool);
    auto contr = create_eurusd(pool);
    auto cpty = create_scb(pool);
    auto now = sc_millis();

    auto in = create_trade(pool, 1, 2, *trader, *giveup, *contr, sc_ymd_to_jd(2014, 3, 14), "apple",
                           SC_ACTION_BUY, 12345, 10, 0, 10, 12345, 10, 3, SC_ROLE_TAKER,
                           *cpty, now);
    in->i.state = SC_STATE_TRADE;

    auto len = sc_json_exec_len(in.get());
    char buf[len + 1];
    const char* end = sc_json_write_exec(buf, in.get());
    check(buf + len == end);
    check(buf[0] == '{');
    check(buf[len - 1] == '}');
}
