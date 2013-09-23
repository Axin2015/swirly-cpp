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
#ifndef DBRPP_ZMQSTORE_HPP
#define DBRPP_ZMQSTORE_HPP

#include <dbrpp/except.hpp>

#include <dbr/zmqstore.h>

namespace dbr {

class ZmqStore {
    DbrZmqStore impl_;
public:
    ~ZmqStore() noexcept
    {
        if (impl_)
            dbr_zmqstore_destroy(impl_);
    }
    constexpr
    ZmqStore(decltype(nullptr)) noexcept
        : impl_(nullptr)
    {
    }
    ZmqStore(void* ctx, DbrIden seed, const char* addr)
        : impl_(dbr_zmqstore_create(ctx, seed, addr))
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrZmqStore() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    ZmqStore(const ZmqStore&) = delete;

    ZmqStore&
    operator =(const ZmqStore&) = delete;

    // Move semantics.

    ZmqStore(ZmqStore&& rhs) noexcept
    : impl_(nullptr)
    {
        swap(rhs);
    }
    ZmqStore&
    operator =(ZmqStore&& rhs) noexcept
    {
        if (impl_) {
            dbr_zmqstore_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(ZmqStore& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    DbrJourn
    journ() const noexcept
    {
        return dbr_zmqstore_journ(impl_);
    }
    DbrModel
    model() const noexcept
    {
        return dbr_zmqstore_model(impl_);
    }
};

} // dbr

#endif // DBRPP_ZMQSTORE_HPP
