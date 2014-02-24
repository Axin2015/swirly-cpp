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
#ifndef DBRPP_ASYNC_HPP
#define DBRPP_ASYNC_HPP

#include <dbrpp/except.hpp>

#include <dbr/async.h>

namespace dbr {

/**
 * @addtogroup Async
 * @{
 */

class Async {
    DbrAsync impl_;
public:
    ~Async() noexcept
    {
        if (impl_)
            dbr_async_destroy(impl_);
    }
    constexpr
    Async(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    Async(void* ctx, const char* sess)
        : impl_{dbr_async_create(ctx, sess)}
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrAsync() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Async(const Async&) = delete;

    Async&
    operator =(const Async&) = delete;

    // Move semantics.

    Async(Async&& rhs) noexcept
        : impl_{nullptr}
    {
        swap(rhs);
    }
    Async&
    operator =(Async&& rhs) noexcept
    {
        if (impl_) {
            dbr_async_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(Async& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    void
    send(DbrAsync async, void* val)
    {
        if (!dbr_async_send(async, val))
            throw_exception();
    }
    void*
    recv(DbrAsync async)
    {
        void* val;
        if (!dbr_async_send(async, &val))
            throw_exception();
        return val;
    }
};

/** @} */

} // dbr

#endif // DBRPP_ASYNC_HPP
