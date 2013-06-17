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
#ifndef DBR_CTX_HPP
#define DBR_CTX_HPP

#include <dbr/except.hpp>
#include <dbr/rec.hpp>

namespace dbr {

class Ctx {
    DbrCtx impl_;
public:
    ~Ctx() noexcept
    {
        if (impl_)
            dbr_ctx_destroy(impl_);
    }
    constexpr
    Ctx(std::nullptr_t) noexcept
    : impl_(nullptr)
    {
    }
    Ctx(DbrPool pool, DbrModel model)
        : impl_(dbr_ctx_create(pool, model))
    {
        if (!impl_)
            throw_exception();
    }

    // Copy semantics.

    Ctx(const Ctx&) = delete;

    Ctx&
    operator =(const Ctx&) = delete;

    // Move semantics.

    Ctx(Ctx&& rhs) noexcept
    : impl_(nullptr)
    {
        std::swap(impl_, rhs.impl_);
    }
    Ctx&
    operator =(Ctx&& rhs) noexcept
    {
        if (impl_) {
            dbr_ctx_destroy(impl_);
            impl_ = nullptr;
        }
        std::swap(impl_, rhs.impl_);
        return *this;
    }
    void
    swap(Ctx& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    operator DbrCtx() noexcept
    {
        return impl_;
    }
    InstrRecs
    instr_recs() const noexcept
    {
        return InstrRecs(impl_);
    }
    MarketRecs
    market_recs() const noexcept
    {
        return MarketRecs(impl_);
    }
    TraderRecs
    trader_recs() const noexcept
    {
        return TraderRecs(impl_);
    }
    AccntRecs
    accnt_recs() const noexcept
    {
        return AccntRecs(impl_);
    }
};

} // dbr

#endif // DBR_CTX_HPP
