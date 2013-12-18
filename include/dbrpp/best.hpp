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
#ifndef DBRPP_BEST_HPP
#define DBRPP_BEST_HPP

#include <dbr/book.h>

#include <iostream>

namespace dbr {

class Best {
    DbrBest impl_;
public:
    operator DbrBest&() noexcept
    {
        return impl_;
    }
    DbrBest*
    c_arg() noexcept
    {
        return &impl_;
    }
    DbrTicks
    bid_ticks() const noexcept
    {
        return impl_.bid_ticks;
    }
    DbrLots
    bid_lots() const noexcept
    {
        return impl_.bid_lots;
    }
    size_t
    bid_count() const noexcept
    {
        return impl_.bid_count;
    }
    DbrTicks
    ask_ticks() const noexcept
    {
        return impl_.ask_ticks;
    }
    DbrLots
    ask_lots() const noexcept
    {
        return impl_.ask_lots;
    }
    size_t
    ask_count() const noexcept
    {
        return impl_.ask_count;
    }
};

inline std::ostream&
operator <<(std::ostream& os, const Best& best)
{
    return os << "bid_ticks=" << best.bid_ticks()
              << ",bid_lots=" << best.bid_lots()
              << ",bid_count=" << best.bid_count()
              << ",ask_ticks=" << best.ask_ticks()
              << ",ask_lots=" << best.ask_lots()
              << ",ask_count=" << best.ask_count();
}

inline Best
best(DbrBook& book)
{
    Best best;
    dbr_book_best(&book, best.c_arg());
    return best;
}
} // dbr

#endif // DBRPP_BEST_HPP
