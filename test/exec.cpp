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
#include "journ.hpp"
#include "model.hpp"
#include "test.hpp"

#include <dbrpp/ctx.hpp>
#include <dbrpp/pool.hpp>

using namespace dbr;
using namespace std;

TEST_CASE(submit)
{
    Journ journ(1);
    Model model;
    Pool pool;
    Ctx ctx(&journ, &model, pool);

    auto tit = ctx.trecs().find("WRAMIREZ");
    check(tit != ctx.trecs().end());

    auto ait = ctx.arecs().find("DBRA");
    check(ait != ctx.arecs().end());

    auto cit = ctx.crecs().find("EURUSD");
    check(cit != ctx.crecs().end());

    auto book = ctx.book(ContrRecRef(*cit), 20130824);

    Trans trans(ctx);
    ctx.place(TraderRecRef(*tit), AccntRecRef(*ait), book, nullptr,
              DBR_BUY, 12345, 1, 0, 0, trans);
}
