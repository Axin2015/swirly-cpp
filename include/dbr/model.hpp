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
#ifndef DBR_MODEL_HPP
#define DBR_MODEL_HPP

#include <dbr/except.hpp>
#include <dbr/model.h>

#include <cstddef>

namespace dbr {

class SqliteModel {
    DbrModel impl_;
public:
    ~SqliteModel() noexcept
    {
        if (impl_)
            dbr_sqlite_destroy(impl_);
    }
    constexpr
    SqliteModel(std::nullptr_t) noexcept
    : impl_(nullptr)
    {
    }
    SqliteModel(DbrPool pool, DbrIden seed, const char* path)
        : impl_(dbr_sqlite_create(pool, seed, path))
    {
        if (!impl_)
            throw_exception();
    }

    // Copy semantics.

    SqliteModel(const SqliteModel&) = delete;

    SqliteModel&
    operator =(const SqliteModel&) = delete;

    // Move semantics.

    SqliteModel(SqliteModel&& rhs) noexcept
    : impl_(nullptr)
    {
        std::swap(impl_, rhs.impl_);
    }
    SqliteModel&
    operator =(SqliteModel&& rhs) noexcept
    {
        if (impl_) {
            dbr_sqlite_destroy(impl_);
            impl_ = nullptr;
        }
        std::swap(impl_, rhs.impl_);
        return *this;
    }
    void
    swap(SqliteModel& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    operator DbrModel() noexcept
    {
        return impl_;
    }
};

} // dbr

#endif // DBR_MODEL_HPP
