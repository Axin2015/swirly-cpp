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

#include <dbrpp/level.hpp>
#include <dbrpp/order.hpp>
#include <dbrpp/pool.hpp>
#include <dbrpp/side.hpp>

#include <dbr/util.h>

using namespace dbr;

TEST_CASE(side_orders)
{
    Pool pool;
    auto trader = create_wramirez(pool);
    auto accnt = create_dbra(pool);
    auto contr = create_eurusd(pool);
    auto now = dbr_millis();

    // Two orders at the same price level.
    auto apple = create_order(pool, 1, *trader, *accnt, *contr, 20130827,
                              "apple", DBR_BUY, 12345, 10, 0, now);
    auto orange = create_order(pool, 2, *trader, *accnt, *contr, 20130827,
                               "orange", DBR_BUY, 12345, 20, 0, now);

    Side side(pool);

    apple->c.rev = -1;
    apple->c.status = -1;
    apple->c.resd = -1;
    apple->c.exec = -1;
    apple->c.last_ticks = -1;
    apple->c.last_lots = -1;

    orange->c.rev = -1;
    orange->c.status = -1;
    orange->c.resd = -1;
    orange->c.exec = -1;
    orange->c.last_ticks = -1;
    orange->c.last_lots = -1;

    // Place orders.
    side.place_order(*apple, dbr_millis());
    side.place_order(*orange, dbr_millis());

    check(apple->c.rev == 1);
    check(apple->c.status == DBR_NEW);
    check(apple->c.resd == 10);
    check(apple->c.exec == 0);
    check(apple->c.last_ticks == -1);
    check(apple->c.last_lots == -1);

    check(side.levels().size() == 1);
    check(side.orders().size() == 2);

    check(OrderRef(side.orders().front()).ref().str() == "apple");
    check(OrderRef(side.orders().back()).ref().str() == "orange");

    auto level = LevelRef(side.levels().front());
    // Two orders at this price level.
    check(level.count() == 2);
    check(level.ticks() == 12345);
    // Sum of lots.
    check(level.resd() == 30);

    // Revise first order.
    side.revise_order(*apple, 5, dbr_millis());

    check(apple->c.rev == 2);
    check(apple->c.status == DBR_REVISE);
    check(apple->c.resd == 5);
    check(apple->c.exec == 0);
    check(apple->c.last_ticks == -1);
    check(apple->c.last_lots == -1);

    check(side.levels().size() == 1);
    check(side.orders().size() == 2);

    check(OrderRef(side.orders().front()).ref().str() == "apple");
    check(OrderRef(side.orders().back()).ref().str() == "orange");

    level = LevelRef(side.levels().front());
    check(level.count() == 2);
    check(level.ticks() == 12345);
    check(level.resd() == 25);

    // Cancel second order.
    side.cancel_order(*orange, dbr_millis());

    check(orange->c.rev == 2);
    check(orange->c.status == DBR_CANCEL);
    check(orange->c.resd == 0);
    check(orange->c.exec == 0);
    check(orange->c.last_ticks == -1);
    check(orange->c.last_lots == -1);

    check(side.levels().size() == 1);
    check(side.orders().size() == 1);

    check(OrderRef(side.orders().front()).ref().str() == "apple");
    check(OrderRef(side.orders().back()).ref().str() == "apple");

    level = LevelRef(side.levels().front());
    check(level.count() == 1);
    check(level.ticks() == 12345);
    check(level.resd() == 5);
}

TEST_CASE(side_levels)
{
    Pool pool;
    auto trader = create_wramirez(pool);
    auto accnt = create_dbra(pool);
    auto contr = create_eurusd(pool);
    auto now = dbr_millis();

    auto apple = create_order(pool, 1, *trader, *accnt, *contr, 20130827,
                              "apple", DBR_BUY, 12345, 10, 0, now);
    auto orange = create_order(pool, 2, *trader, *accnt, *contr, 20130827,
                               "orange", DBR_BUY, 12345, 20, 0, now);
    // Best inserted last.
    auto pear = create_order(pool, 3, *trader, *accnt, *contr, 20130827,
                             "pear", DBR_BUY, 12346, 30, 0, now);

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
    check(level.count() == 1);
    check(level.ticks() == 12346);
    check(level.resd() == 30);

    // Level 2.

    level = LevelRef(side.levels().back());
    check(level.count() == 2);
    check(level.ticks() == 12345);
    check(level.resd() == 30);
}
