/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "factory.hpp"
#include "test.hpp"

#include <scpp/elm/level.hpp>
#include <scpp/elm/order.hpp>
#include <scpp/elm/pool.hpp>
#include <scpp/elm/side.hpp>

#include <sc/ash/util.h>

using namespace sc;

TEST_CASE(side_orders)
{
    Pool pool(8 * 1024 * 1024);
    auto trader = create_wramirez(pool);
    auto giveup = create_sca(pool);
    auto contr = create_eurusd(pool);
    auto now = sc_millis();

    // Two orders at the same price level.
    auto apple = create_order(pool, 1, *trader, *giveup, *contr, sc_ymd_to_jd(2014, 3, 14),
                              "apple", SC_ACTION_BUY, 12345, 10, 0, now);
    auto orange = create_order(pool, 2, *trader, *giveup, *contr, sc_ymd_to_jd(2014, 3, 14),
                               "orange", SC_ACTION_BUY, 12345, 20, 0, now);

    Side side(pool);

    apple->i.state = -1;
    apple->i.resd = -1;
    apple->i.exec = -1;
    apple->i.last_ticks = -1;
    apple->i.last_lots = -1;

    orange->i.state = -1;
    orange->i.resd = -1;
    orange->i.exec = -1;
    orange->i.last_ticks = -1;
    orange->i.last_lots = -1;

    // Place orders.
    side.place_order(*apple, now);
    side.place_order(*orange, now);

    check(apple->i.state == SC_STATE_NEW);
    check(apple->i.resd == 10);
    check(apple->i.exec == 0);
    check(apple->i.last_ticks == -1);
    check(apple->i.last_lots == -1);

    check(side.levels().size() == 1);
    check(side.orders().size() == 2);

    check(OrderRef(side.orders().front()).ref().str() == "apple");
    check(OrderRef(side.orders().back()).ref().str() == "orange");

    auto level = LevelRef(side.levels().front());
    check(level.ticks() == 12345);
    // Sum of lots.
    check(level.lots() == 30);
    // Two orders at this price level.
    check(level.count() == 2);

    // Revise first order.
    side.revise_order(*apple, 5, sc_millis());

    check(apple->i.state == SC_STATE_REVISE);
    check(apple->i.resd == 5);
    check(apple->i.exec == 0);
    check(apple->i.last_ticks == -1);
    check(apple->i.last_lots == -1);

    check(side.levels().size() == 1);
    check(side.orders().size() == 2);

    check(OrderRef(side.orders().front()).ref().str() == "apple");
    check(OrderRef(side.orders().back()).ref().str() == "orange");

    level = LevelRef(side.levels().front());
    check(level.ticks() == 12345);
    check(level.lots() == 25);
    check(level.count() == 2);

    // Cancel second order.
    side.cancel_order(*orange, sc_millis());

    check(orange->i.state == SC_STATE_CANCEL);
    check(orange->i.resd == 0);
    check(orange->i.exec == 0);
    check(orange->i.last_ticks == -1);
    check(orange->i.last_lots == -1);

    check(side.levels().size() == 1);
    check(side.orders().size() == 1);

    check(OrderRef(side.orders().front()).ref().str() == "apple");
    check(OrderRef(side.orders().back()).ref().str() == "apple");

    level = LevelRef(side.levels().front());
    check(level.ticks() == 12345);
    check(level.lots() == 5);
    check(level.count() == 1);
}

TEST_CASE(side_levels)
{
    Pool pool(8 * 1024 * 1024);
    auto trader = create_wramirez(pool);
    auto giveup = create_sca(pool);
    auto contr = create_eurusd(pool);
    auto now = sc_millis();

    auto apple = create_order(pool, 1, *trader, *giveup, *contr, sc_ymd_to_jd(2014, 3, 14),
                              "apple", SC_ACTION_BUY, 12345, 10, 0, now);
    auto orange = create_order(pool, 2, *trader, *giveup, *contr, sc_ymd_to_jd(2014, 3, 14),
                               "orange", SC_ACTION_BUY, 12345, 20, 0, now);
    // Best inserted last.
    auto pear = create_order(pool, 3, *trader, *giveup, *contr, sc_ymd_to_jd(2014, 3, 14),
                             "pear", SC_ACTION_BUY, 12346, 30, 0, now);

    Side side(pool);
    side.place_order(*apple, sc_millis());
    side.place_order(*orange, sc_millis());
    side.place_order(*pear, sc_millis());

    check(side.levels().size() == 2);
    check(side.orders().size() == 3);

    check(OrderRef(side.orders().front()).ref().str() == "pear");
    check(OrderRef(side.orders().back()).ref().str() == "orange");

    // Level 1.

    auto level = LevelRef(side.levels().front());
    check(level.ticks() == 12346);
    check(level.lots() == 30);
    check(level.count() == 1);

    // Level 2.

    level = LevelRef(side.levels().back());
    check(level.ticks() == 12345);
    check(level.lots() == 30);
    check(level.count() == 2);
}
