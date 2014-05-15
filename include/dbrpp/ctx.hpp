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
#ifndef DBRPP_CTX_HPP
#define DBRPP_CTX_HPP

#include <dbrpp/async.hpp>

#include <dbr/ctx.h>

namespace dbr {

/**
 * @addtogroup Ctx
 * @{
 */

class Ctx {
    DbrCtx impl_;
public:
    ~Ctx() noexcept
    {
        if (impl_)
            dbr_ctx_destroy(impl_);
    }
    constexpr
    Ctx(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    Ctx(const char* mdaddr, const char* traddr, DbrIden seed, DbrMillis tmout, size_t capacity,
        DbrHandler handler)
        : impl_{dbr_ctx_create(mdaddr, traddr, seed, tmout, capacity, handler)}
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrCtx() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Ctx(const Ctx&) = delete;

    Ctx&
    operator =(const Ctx&) = delete;

    // Move semantics.

    Ctx(Ctx&& rhs) noexcept
        : impl_{nullptr}
    {
        swap(rhs);
    }
    Ctx&
    operator =(Ctx&& rhs) noexcept
    {
        if (impl_) {
            dbr_ctx_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(Ctx& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    Async
    async()
    {
        DbrAsync impl = dbr_ctx_async(impl_);
        if (!impl)
            throw_exception();
        return Async(impl);
    }
};

/** @} */

} // dbr

#endif // DBRPP_CTX_HPP
