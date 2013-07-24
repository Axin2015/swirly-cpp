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

#include <dbr/conv.h>
#include <dbr/trader.h>

using namespace dbrpp;

TEST_CASE(trader_id)
{
    Pool pool;
    Journ journ(1);
    Model model(pool);
    Ctx ctx(pool, &journ, &model);

    TraderRecs::Iterator it = ctx.trecs().find("WRAMIREZ");
    check(it != ctx.trecs().end());

    TraderRec trec(*it);
    Trader trader = ctx.trader(*it);
    check(trader.id() == trec.id());
}
