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
#include <dbr/conv.h>

#include <iostream>

namespace dbr {

class BookRef {
    DbrBook* impl_;
public:
    explicit
    BookRef(DbrBook& impl) noexcept
        : impl_{&impl}
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
    operator ==(BookRef rhs) const noexcept
    {
        return key() == rhs.key();
    }
    bool
    operator !=(BookRef rhs) const noexcept
    {
        return key() != rhs.key();
    }
    DbrKey
    key() const noexcept
    {
        return dbr_book_key(impl_);
    }
    ContrRecRef
    crec() const noexcept
    {
        return ContrRecRef{*dbr_book_crec(impl_)};
    }
    DbrDate
    settl_date() const noexcept
    {
        return dbr_book_settl_date(impl_);
    }
    SideRef
    bid_side() const noexcept
    {
        return SideRef{*dbr_book_bid_side(impl_)};
    }
    SideRef
    ask_side() const noexcept
    {
        return SideRef{*dbr_book_ask_side(impl_)};
    }
};

inline std::ostream&
operator <<(std::ostream& os, BookRef book)
{
    return os << "key=" << book.key()
              << ",crec=" << book.crec().mnem()
              << ",settl_date" << book.settl_date();
}

} // dbr

#endif // DBRPP_BOOK_HPP
