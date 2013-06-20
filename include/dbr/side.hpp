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
#ifndef DBR_SIDE_HPP
#define DBR_SIDE_HPP

#include <dbr/side.h>

#include <iostream>

namespace dbr {

class Side {
    DbrSide impl_;
public:
    Side(DbrSide impl) noexcept
        : impl_(impl)
    {
    }
    DbrTicks
    last_ticks() const noexcept
    {
        return dbr_side_last_ticks(impl_);
    }
    DbrLots
    last_lots() const noexcept
    {
        return dbr_side_last_lots(impl_);
    }
    DbrMillis
    last_time() const noexcept
    {
        return dbr_side_last_time(impl_);
    }
};

inline std::ostream&
operator <<(std::ostream& os, Side side)
{
    return os << "last_ticks=" << side.last_ticks()
              << ",last_lots=" << side.last_lots()
              << ",last_time=" << side.last_time();
}

} // dbr

#endif // DBR_SIDE_HPP
