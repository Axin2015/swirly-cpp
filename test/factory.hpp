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
#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <dbr/types.h>

#include <memory>

namespace dbr {
class Pool;
}

std::shared_ptr<DbrRec>
create_trader(dbr::Pool& pool, DbrIden id, const char* mnem, const char* display,
              const char* email);

std::shared_ptr<DbrRec>
create_accnt(dbr::Pool& pool, DbrIden id, const char* mnem, const char* display,
             const char* email);

std::shared_ptr<DbrRec>
create_contr(dbr::Pool& pool, DbrIden id, const char* mnem, const char* display,
             const char* asset_type, const char* asset, const char* ccy, int tick_numer,
             int tick_denom, int lot_numer, int lot_denom, int pip_dp, DbrLots min_lots,
             DbrLots max_lots);

std::shared_ptr<DbrOrder>
create_order(dbr::Pool& pool, DbrIden id, DbrRec& trader, DbrRec& accnt, DbrRec& contr,
             DbrDate settl_date, const char* ref, int action, DbrTicks ticks, DbrLots lots,
             DbrLots min, DbrFlags flags);

#endif // FACTORY_HPP
