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
#ifndef DBRPP_SQLITE_HPP
#define DBRPP_SQLITE_HPP

#include <dbrpp/except.hpp>

#include <dbr/sqlite.h>

namespace dbrpp {

class Sqlite {
    DbrSqlite impl_;
public:
    ~Sqlite() noexcept
    {
        if (impl_)
            dbr_sqlite_destroy(impl_);
    }
    constexpr
    Sqlite(std::nullptr_t) noexcept
    : impl_(nullptr)
    {
    }
    Sqlite(DbrPool pool, DbrIden seed, const char* path)
    : impl_(dbr_sqlite_create(pool, seed, path))
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrSqlite() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Sqlite(const Sqlite&) = delete;

    Sqlite&
    operator =(const Sqlite&) = delete;

    // Move semantics.

    Sqlite(Sqlite&& rhs) noexcept
    : impl_(nullptr)
    {
        std::swap(impl_, rhs.impl_);
    }
    Sqlite&
    operator =(Sqlite&& rhs) noexcept
    {
        if (impl_) {
            dbr_sqlite_destroy(impl_);
            impl_ = nullptr;
        }
        std::swap(impl_, rhs.impl_);
        return *this;
    }
    void
    swap(Sqlite& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    DbrJourn
    journ() const noexcept
    {
        return dbr_sqlite_journ(impl_);
    }
    DbrModel
    model() const noexcept
    {
        return dbr_sqlite_model(impl_);
    }
};

} // dbrpp

#endif // DBRPP_SQLITE_HPP
