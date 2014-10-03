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

#include <dbrpp/fig/serv.hpp>

#include <dbrpp/elm/pool.hpp>

using namespace dbr;

template <int TypeN>
struct TypeTraits;

template <>
struct TypeTraits<DBR_ENTITY_ACCNT> {
    typedef AccntRecRef TypeRecRef;
};

template <>
struct TypeTraits<DBR_ENTITY_CONTR> {
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

TEST_CASE(serv_accnt)
{
    Journ journ;
    Model model;
    Pool pool(8 * 1024 * 1024);
    Serv serv(nullptr, &journ, pool);
    serv.load(&model);

    check(serv.arecs().find("BAD") == serv.arecs().end());

    AccntRecRef arec = get_rec_mnem<DBR_ENTITY_ACCNT>(serv, "DBRA");
    check(arec == get_rec_id<DBR_ENTITY_ACCNT>(serv, arec.id()));
    check(arec.mnem() == "DBRA");

    // Body.
    check(arec.display() == "Account A");
    check(arec.email() == "dbra@doobry.org");
}

TEST_CASE(serv_contr)
{
    Journ journ;
    Model model;
    Pool pool(8 * 1024 * 1024);
    Serv serv(nullptr, &journ, pool);
    serv.load(&model);

    check(serv.crecs().find("BAD") == serv.crecs().end());

    ContrRecRef crec = get_rec_mnem<DBR_ENTITY_CONTR>(serv, "EURUSD");
    check(crec == get_rec_id<DBR_ENTITY_CONTR>(serv, crec.id()));
    check(crec.mnem() == "EURUSD");

    // Body.
    check(crec.display() == "EURUSD");
    check(crec.asset_type() == "CURRENCY");
    check(crec.asset() == "EUR");
    check(crec.ccy() == "USD");
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
    Journ journ;
    Model model;
    Pool pool(8 * 1024 * 1024);
    Serv serv(nullptr, &journ, pool);
    serv.load(&model);

    ServContrRecs::Iterator it = serv.crecs().find("EURUSD");
    check(it != serv.crecs().end());

    ContrRecRef crec(*it);
    BookRef book = serv.book(*it, 2456731);
    check(book.crec() == crec);
    check(book.settl_day() == 2456731);
}

TEST_CASE(serv_place)
{
    Journ journ;
    Model model;
    Pool pool(8 * 1024 * 1024);
    Serv serv(nullptr, &journ, pool);
    serv.load(&model);

    auto uit = serv.arecs().find("WRAMIREZ");
    check(uit != serv.arecs().end());

    auto git = serv.arecs().find("DBRA");
    check(git != serv.arecs().end());

    auto cit = serv.crecs().find("EURUSD");
    check(cit != serv.crecs().end());

    auto trader = serv.accnt(AccntRecRef(*uit));
    auto giveup = serv.accnt(AccntRecRef(*git));
    auto book = serv.book(ContrRecRef(*cit), dbr_ymd_to_jd(2014, 3, 14));

    DbrUuid uuid;
    uuid_generate(uuid);
    auto sess = serv.sess(uuid);
    sess.logon(trader);

    serv.place(trader, giveup, book, nullptr, DBR_ACTION_BUY, 12345, 1, 0);
}
