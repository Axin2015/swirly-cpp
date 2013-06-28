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
#include "test.hpp"
#include "model.hpp"

#include <dbr/ctx.hpp>
#include <dbr/pool.hpp>

using namespace dbr;
using namespace std;

TEST_CASE(submit)
{
    Pool pool;
    Model model(pool, 1);
    Ctx ctx(pool, &model);

    auto tit = ctx.trecs().find("WRAMIREZ");
    check(tit != ctx.trecs().end());

    auto ait = ctx.arecs().find("DBRA");
    check(ait != ctx.arecs().end());

    auto mit = ctx.mrecs().find("EURUSD");
    check(mit != ctx.mrecs().end());

    Trans trans(ctx);
    ctx.submit(TraderRec(*tit), AccntRec(*ait), nullptr, MarketRec(*mit),
               DBR_BUY, 12345, 1, 0, 0, trans);
}
