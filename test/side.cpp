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

#include <dbrpp/pool.hpp>
#include <dbrpp/side.hpp>

#include <dbr/util.h>

using namespace dbr;

TEST_CASE(side_test)
{
    Pool pool;
    auto trader = create_trader(pool, 1, "WRAMIREZ", "Wayne Ramirez", "wayne.ramirez@doobry.org");
    auto accnt = create_accnt(pool, 1, "DBRA", "Account A", "dbra@doobry.org");
    auto contr = create_contr(pool, 1, "EURUSD", "EURUSD", "CURRENCY", "EUR", "USD",
                              1, 10000, 1000000, 1, 4, 1, 10);

    auto apple = create_order(pool, 1, *trader, *accnt, *contr, 20130827,
                              "apple", DBR_BUY, 12345, 10, 0, 0);
    auto orange = create_order(pool, 2, *trader, *accnt, *contr, 20130827,
                               "orange", DBR_BUY, 12345, 10, 0, 0);
    auto pear = create_order(pool, 3, *trader, *accnt, *contr, 20130827,
                             "pear", DBR_BUY, 12345, 10, 0, 0);

    Side side(pool);
    side.new_order(*apple, dbr_millis());
    side.new_order(*orange, dbr_millis());
    side.new_order(*pear, dbr_millis());

    check(side.levels().size() == 1);
    check(side.orders().size() == 3);
}
