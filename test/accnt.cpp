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

#include <dbr/accnt.h>

using namespace dbr;

TEST_CASE(model_accnt_id)
{
    Model model;
    Pool pool;
    auto recs = read_entity<DBR_ACCNT>(&model, pool);
    auto it = std::find_if(recs.begin(), recs.end(), [](const DbrRec& rec) {
            return strncmp(rec.mnem, "DBRA", DBR_MNEM_MAX) == 0;
        });
    check(it != recs.end());
}

TEST_CASE(exch_accnt_id)
{
    Journ journ(1);
    Model model;
    Pool pool;
    Exch exch(&journ, &model, pool);

    ExchAccntRecs::Iterator it = exch.arecs().find("DBRA");
    check(it != exch.arecs().end());

    AccntRecRef arec(*it);
    Accnt accnt = exch.accnt(*it);
    check(accnt.id() == arec.id());
}
