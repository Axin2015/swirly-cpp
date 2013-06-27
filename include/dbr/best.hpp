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
#ifndef DBR_BEST_HPP
#define DBR_BEST_HPP

#include <dbr/types.h>

#include <iostream>

namespace dbr {

class Best {
    DbrBest impl_;
public:
    explicit
    operator DbrBest*() noexcept
    {
        return &impl_;
    }
    DbrTicks
    bid_ticks() const noexcept
    {
        return impl_.bid_ticks;
    }
    DbrLots
    bid_resd() const noexcept
    {
        return impl_.bid_resd;
    }
    DbrTicks
    ask_ticks() const noexcept
    {
        return impl_.ask_ticks;
    }
    DbrLots
    ask_resd() const noexcept
    {
        return impl_.ask_resd;
    }
};

inline std::ostream&
operator <<(std::ostream& os, const Best& best)
{
    return os << "bid_ticks=" << best.bid_ticks()
              << ",bid_resd=" << best.bid_resd()
              << ",ask_ticks=" << best.ask_ticks()
              << ",ask_resd=" << best.ask_resd();
}
} // dbr

#endif // DBR_BEST_HPP
