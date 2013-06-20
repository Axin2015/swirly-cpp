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
#ifndef DBR_TRADER_HPP
#define DBR_TRADER_HPP

#include <dbr/trader.h>

#include <iostream>

namespace dbr {

class Trader {
    DbrTrader impl_;
public:
    Trader(DbrTrader impl) noexcept
        : impl_(impl)
    {
    }
    bool
    operator ==(const Trader& rhs) const noexcept
    {
        return id() == rhs.id();
    }
    bool
    operator !=(const Trader& rhs) const noexcept
    {
        return id() != rhs.id();
    }
    DbrIden
    id() const noexcept
    {
        return dbr_trader_id(impl_);
    }
};

inline std::ostream&
operator <<(std::ostream& os, Trader trader)
{
    return os << "id=" << trader.id();
}

} // dbr

#endif // DBR_TRADER_HPP
