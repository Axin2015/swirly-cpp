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
#include "test.hpp"

#include <dbrpp/elm/level.hpp>
#include <dbrpp/elm/order.hpp>
#include <dbrpp/elm/pool.hpp>
#include <dbrpp/fig/side.hpp>

#include <dbr/ash/util.h>

using namespace dbr;

TEST_CASE(side_orders)
{
    Pool pool(8 * 1024 * 1024);
    auto user = create_wramirez(pool);
    auto group = create_dbra(pool);
    auto contr = create_eurusd(pool);
    auto now = dbr_millis();

    // Two orders at the same price level.
    auto apple = create_order(pool, 1, *user, *group, *contr, dbr_ymd_to_jd(2014, 3, 14),
                              "apple", DBR_ACTION_BUY, 12345, 10, 0, now);
    auto orange = create_order(pool, 2, *user, *group, *contr, dbr_ymd_to_jd(2014, 3, 14),
                               "orange", DBR_ACTION_BUY, 12345, 20, 0, now);

    Side side(pool);

    apple->c.state = -1;
    apple->c.resd = -1;
    apple->c.exec = -1;
    apple->c.last_ticks = -1;
    apple->c.last_lots = -1;

    orange->c.state = -1;
    orange->c.resd = -1;
    orange->c.exec = -1;
    orange->c.last_ticks = -1;
    orange->c.last_lots = -1;

    // Place orders.
    side.place_order(*apple, dbr_millis());
    side.place_order(*orange, dbr_millis());

    check(apple->c.state == DBR_STATE_NEW);
    check(apple->c.resd == 10);
    check(apple->c.exec == 0);
    check(apple->c.last_ticks == -1);
    check(apple->c.last_lots == -1);

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
    side.revise_order(*apple, 5, dbr_millis());

    check(apple->c.state == DBR_STATE_REVISE);
    check(apple->c.resd == 5);
    check(apple->c.exec == 0);
    check(apple->c.last_ticks == -1);
    check(apple->c.last_lots == -1);

    check(side.levels().size() == 1);
    check(side.orders().size() == 2);

    check(OrderRef(side.orders().front()).ref().str() == "apple");
    check(OrderRef(side.orders().back()).ref().str() == "orange");

    level = LevelRef(side.levels().front());
    check(level.ticks() == 12345);
    check(level.lots() == 25);
    check(level.count() == 2);

    // Cancel second order.
    side.cancel_order(*orange, dbr_millis());

    check(orange->c.state == DBR_STATE_CANCEL);
    check(orange->c.resd == 0);
    check(orange->c.exec == 0);
    check(orange->c.last_ticks == -1);
    check(orange->c.last_lots == -1);

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
    auto user = create_wramirez(pool);
    auto group = create_dbra(pool);
    auto contr = create_eurusd(pool);
    auto now = dbr_millis();

    auto apple = create_order(pool, 1, *user, *group, *contr, dbr_ymd_to_jd(2014, 3, 14),
                              "apple", DBR_ACTION_BUY, 12345, 10, 0, now);
    auto orange = create_order(pool, 2, *user, *group, *contr, dbr_ymd_to_jd(2014, 3, 14),
                               "orange", DBR_ACTION_BUY, 12345, 20, 0, now);
    // Best inserted last.
    auto pear = create_order(pool, 3, *user, *group, *contr, dbr_ymd_to_jd(2014, 3, 14),
                             "pear", DBR_ACTION_BUY, 12346, 30, 0, now);

    Side side(pool);
    side.place_order(*apple, dbr_millis());
    side.place_order(*orange, dbr_millis());
    side.place_order(*pear, dbr_millis());

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
