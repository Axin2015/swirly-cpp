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
#ifndef DBRPP_SQLSTORE_HPP
#define DBRPP_SQLSTORE_HPP

#include <dbrpp/except.hpp>

#include <dbr/sqlstore.h>

namespace dbr {

class SqlStore {
    DbrSqlStore impl_;
public:
    ~SqlStore() noexcept
    {
        if (impl_)
            dbr_sqlstore_destroy(impl_);
    }
    constexpr
    SqlStore(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    SqlStore(const char* path, DbrIden seed)
        : impl_{dbr_sqlstore_create(path, seed)}
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrSqlStore() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    SqlStore(const SqlStore&) = delete;

    SqlStore&
    operator =(const SqlStore&) = delete;

    // Move semantics.

    SqlStore(SqlStore&& rhs) noexcept
        : impl_{nullptr}
    {
        swap(rhs);
    }
    SqlStore&
    operator =(SqlStore&& rhs) noexcept
    {
        if (impl_) {
            dbr_sqlstore_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(SqlStore& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    DbrJourn
    journ() const noexcept
    {
        return dbr_sqlstore_journ(impl_);
    }
    DbrModel
    model() const noexcept
    {
        return dbr_sqlstore_model(impl_);
    }
};

} // dbr

#endif // DBRPP_SQLSTORE_HPP
