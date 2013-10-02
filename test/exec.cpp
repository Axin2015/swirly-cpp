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
#include "mock.hpp"
#include "test.hpp"

#include <dbrpp/exch.hpp>
#include <dbrpp/pool.hpp>

using namespace dbr;
using namespace std;

TEST_CASE(exec_book)
{
    Journ journ(1);
    Model model;
    Pool pool;
    Exch exch(&journ, &model, pool);

    ExchContrRecs::Iterator it = exch.crecs().find("EURUSD");
    check(it != exch.crecs().end());

    ContrRecRef crec(*it);
    BookRef book = exch.book(*it, 20130824);
    check(book.crec() == crec);
    check(book.settl_date() == 20130824);
}

TEST_CASE(exec_place)
{
    Journ journ(1);
    Model model;
    Pool pool;
    Exch exch(&journ, &model, pool);

    auto tit = exch.trecs().find("WRAMIREZ");
    check(tit != exch.trecs().end());

    auto ait = exch.arecs().find("DBRA");
    check(ait != exch.arecs().end());

    auto cit = exch.crecs().find("EURUSD");
    check(cit != exch.crecs().end());

    auto trader = exch.trader(TraderRecRef(*tit));
    auto accnt = exch.accnt(AccntRecRef(*ait));
    auto book = exch.book(ContrRecRef(*cit), 20130824);

    Result result;
    exch.place(trader, accnt, book, nullptr, DBR_BUY, 12345, 1, 0, 0, result);
}
