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
#ifndef DBRPP_BOOK_HPP
#define DBRPP_BOOK_HPP

#include <dbrpp/rec.hpp>
#include <dbrpp/side.hpp>

#include <dbr/book.h>

#include <iostream>

namespace dbr {

class Book {
    DbrBook* impl_;
public:
    explicit
    Book(DbrBook& impl) noexcept
        : impl_(&impl)
    {
    }
    operator DbrBook&() const noexcept
    {
        return *impl_;
    }
    DbrBook*
    c_arg() const noexcept
    {
        return impl_;
    }
    bool
    operator ==(Book rhs) const noexcept
    {
        return key() == rhs.key();
    }
    bool
    operator !=(Book rhs) const noexcept
    {
        return key() != rhs.key();
    }
    DbrKey
    key() const noexcept
    {
        return dbr_book_key(impl_);
    }
    ContrRec
    crec() const noexcept
    {
        return ContrRec(*dbr_book_crec(impl_));
    }
    DbrDate
    settl_date() const noexcept
    {
        return dbr_book_settl_date(impl_);
    }
    Side
    bid_side() const noexcept
    {
        return Side(*dbr_book_bid_side(impl_));
    }
    Side
    ask_side() const noexcept
    {
        return Side(*dbr_book_ask_side(impl_));
    }
};

inline std::ostream&
operator <<(std::ostream& os, Book book)
{
    return os << "key=" << book.key()
              << ",crec=" << book.crec().mnem()
              << ",settl_date" << book.settl_date();
}

} // dbr

#endif // DBRPP_BOOK_HPP
