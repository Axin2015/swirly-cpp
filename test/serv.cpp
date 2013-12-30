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
#include "mock.hpp"
#include "test.hpp"

#include <dbrpp/pool.hpp>
#include <dbrpp/serv.hpp>

using namespace dbr;

template <int TypeN>
struct TypeTraits;

template <>
struct TypeTraits<DBR_TRADER> {
    typedef TraderRecRef TypeRecRef;
};

template <>
struct TypeTraits<DBR_ACCNT> {
    typedef AccntRecRef TypeRecRef;
};

template <>
struct TypeTraits<DBR_CONTR> {
    typedef ContrRecRef TypeRecRef;
};

template <int TypeN>
static typename TypeTraits<TypeN>::TypeRecRef
get_rec_id(Serv& serv, DbrIden id)
{
    auto it = serv.recs<TypeN>().find(id);
    check(it != serv.recs<TypeN>().end());
    return typename TypeTraits<TypeN>::TypeRecRef(*it);
}

template <int TypeN>
static typename TypeTraits<TypeN>::TypeRecRef
get_rec_mnem(Serv& serv, const char* mnem)
{
    auto it = serv.recs<TypeN>().find(mnem);
    check(it != serv.recs<TypeN>().end());
    return typename TypeTraits<TypeN>::TypeRecRef(*it);
}

TEST_CASE(serv_trader)
{
    Journ journ(1);
    Model model;
    Pool pool;
    Serv serv(&journ, &model, pool);

    check(serv.trecs().find("BAD") == serv.trecs().end());

    TraderRecRef trec = get_rec_mnem<DBR_TRADER>(serv, "WRAMIREZ");
    check(trec == get_rec_id<DBR_TRADER>(serv, trec.id()));
    check(trec.mnem() == Mnem("WRAMIREZ"));

    // Body.
    check(trec.display() == Display("Wayne Ramirez"));
    check(trec.email() == Email("wayne.ramirez@doobry.org"));
}

TEST_CASE(serv_accnt)
{
    Journ journ(1);
    Model model;
    Pool pool;
    Serv serv(&journ, &model, pool);

    check(serv.arecs().find("BAD") == serv.arecs().end());

    AccntRecRef arec = get_rec_mnem<DBR_ACCNT>(serv, "DBRA");
    check(arec == get_rec_id<DBR_ACCNT>(serv, arec.id()));
    check(arec.mnem() == Mnem("DBRA"));

    // Body.
    check(arec.display() == Display("Account A"));
    check(arec.email() == Email("dbra@doobry.org"));
}

TEST_CASE(serv_contr)
{
    Journ journ(1);
    Model model;
    Pool pool;
    Serv serv(&journ, &model, pool);

    check(serv.crecs().find("BAD") == serv.crecs().end());

    ContrRecRef crec = get_rec_mnem<DBR_CONTR>(serv, "EURUSD");
    check(crec == get_rec_id<DBR_CONTR>(serv, crec.id()));
    check(crec.mnem() == Mnem("EURUSD"));

    // Body.
    check(crec.display() == Display("EURUSD"));
    check(crec.asset_type() == Mnem("CURRENCY"));
    check(crec.asset() == Mnem("EUR"));
    check(crec.ccy() == Mnem("USD"));
    check(fequal(crec.price_inc(), 0.0001));
    check(fequal(crec.qty_inc(), 1e6));
    check(crec.price_dp() == 4);
    check(crec.pip_dp() == 4);
    check(crec.qty_dp() == 0);
    check(crec.min_lots() == 1);
    check(crec.max_lots() == 10);
}

TEST_CASE(serv_book)
{
    Journ journ(1);
    Model model;
    Pool pool;
    Serv serv(&journ, &model, pool);

    ServContrRecs::Iterator it = serv.crecs().find("EURUSD");
    check(it != serv.crecs().end());

    ContrRecRef crec(*it);
    BookRef book = serv.book(*it, 20130824);
    check(book.crec() == crec);
    check(book.settl_date() == 20130824);
}

TEST_CASE(serv_place)
{
    Journ journ(1);
    Model model;
    Pool pool;
    Serv serv(&journ, &model, pool);

    auto tit = serv.trecs().find("WRAMIREZ");
    check(tit != serv.trecs().end());

    auto ait = serv.arecs().find("DBRA");
    check(ait != serv.arecs().end());

    auto cit = serv.crecs().find("EURUSD");
    check(cit != serv.crecs().end());

    auto trader = serv.trader(TraderRecRef(*tit));
    auto accnt = serv.accnt(AccntRecRef(*ait));
    auto book = serv.book(ContrRecRef(*cit), 20130824);

    serv.place(trader, accnt, book, nullptr, DBR_BUY, 12345, 1, 0);
}
