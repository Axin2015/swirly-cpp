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
#ifndef DBRPP_STORE_HPP
#define DBRPP_STORE_HPP

#include <dbrpp/except.hpp>

#include <dbr/store.h>

namespace dbr {

/**
 * @addtogroup Store
 * @{
 */

class Store {
    DbrStore impl_;
public:
    ~Store() noexcept
    {
        dbr_store_term(&impl_);
    }
    Store(const char* path, size_t len)
    {
        if (!dbr_store_init(&impl_, path, len))
            throw_exception();
    }
    operator DbrStore&() noexcept
    {
        return impl_;
    }
    DbrStore*
    c_arg() noexcept
    {
        return &impl_;
    }

    // Copy semantics.

    Store(const Store&) = delete;

    Store&
    operator =(const Store&) = delete;

    long
    next(size_t idx) noexcept
    {
        return dbr_store_next(&impl_, idx);
    }
};

/** @} */

} // dbr

#endif // DBRPP_STORE_HPP
