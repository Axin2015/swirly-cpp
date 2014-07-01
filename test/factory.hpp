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
#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <dbr/elm/types.h>

#include <memory>

namespace dbr {
class Pool;
}

std::shared_ptr<DbrRec>
create_accnt(dbr::Pool& pool, DbrIden id, const char* mnem, const char* display,
             const char* email);

std::shared_ptr<DbrRec>
create_contr(dbr::Pool& pool, DbrIden id, const char* mnem, const char* display,
             const char* asset_type, const char* asset, const char* ccy, int tick_numer,
             int tick_denom, int lot_numer, int lot_denom, int pip_dp, DbrLots min_lots,
             DbrLots max_lots);

std::shared_ptr<DbrPerm>
create_perm(dbr::Pool& pool, DbrIden tid, DbrIden gid);

std::shared_ptr<DbrOrder>
create_order(dbr::Pool& pool, DbrIden id, DbrRec& trader, DbrRec& giveup, DbrRec& contr,
             DbrJd settl_day, const char* ref, int action, DbrTicks ticks, DbrLots lots,
             DbrLots min_lots, DbrMillis now);

std::shared_ptr<DbrOrder>
create_order(dbr::Pool& pool, DbrIden id, DbrIden tid, DbrIden gid, DbrIden cid,
             DbrJd settl_day, const char* ref, int action, DbrTicks ticks, DbrLots lots,
             DbrLots min_lots, DbrMillis now);

std::shared_ptr<DbrExec>
create_trade(dbr::Pool& pool, DbrIden id, DbrIden order, DbrRec& trader, DbrRec& giveup,
             DbrRec& contr, DbrJd settl_day, const char* ref, int action, DbrTicks ticks,
             DbrLots lots, DbrLots resd, DbrLots exec, DbrTicks last_ticks, DbrLots last_lots,
             DbrIden match, int role, DbrRec& cpty, DbrMillis now);

std::shared_ptr<DbrExec>
create_trade(dbr::Pool& pool, DbrIden id, DbrIden order, DbrIden tid, DbrIden gid,
             DbrIden cid, DbrJd settl_day, const char* ref, int action, DbrTicks ticks,
             DbrLots lots, DbrLots resd, DbrLots exec, DbrTicks last_ticks, DbrLots last_lots,
             DbrIden match, int role, DbrIden cpty, DbrMillis now);

inline std::shared_ptr<DbrRec>
create_wramirez(dbr::Pool& pool)
{
    return create_accnt(pool, 1, "WRAMIREZ", "Wayne Ramirez", "wayne.ramirez@doobry.org");
}

inline std::shared_ptr<DbrRec>
create_sflores(dbr::Pool& pool)
{
    return create_accnt(pool, 2, "SFLORES", "Steven Flores", "steven.flores@doobry.org");
}

inline std::shared_ptr<DbrRec>
create_dbra(dbr::Pool& pool)
{
    return create_accnt(pool, 3, "DBRA", "Account A", "dbra@doobry.org");
}

inline std::shared_ptr<DbrRec>
create_dbrb(dbr::Pool& pool)
{
    return create_accnt(pool, 4, "DBRB", "Account B", "dbrb@doobry.org");
}

inline std::shared_ptr<DbrRec>
create_eurusd(dbr::Pool& pool)
{
    return create_contr(pool, 1, "EURUSD", "EURUSD", "CURRENCY", "EUR", "USD",
                        1, 10000, 1000000, 1, 4, 1, 10);
}

inline std::shared_ptr<DbrRec>
create_gbpusd(dbr::Pool& pool)
{
    return create_contr(pool, 2, "GBPUSD", "GBPUSD", "CURRENCY", "GBP", "USD",
                        1, 10000, 1000000, 1, 4, 1, 10);
}

#endif // FACTORY_HPP
