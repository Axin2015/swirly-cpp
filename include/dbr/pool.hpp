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
#ifndef DBR_POOL_HPP
#define DBR_POOL_HPP

#include <dbr/except.hpp>

#include <dbr/pool.h>

namespace dbr {

class Pool {
    DbrPool impl_;
public:
    ~Pool() noexcept
    {
        if (impl_)
            dbr_pool_destroy(impl_);
    }
    constexpr
    Pool(std::nullptr_t) noexcept
    : impl_(nullptr)
    {
    }
    Pool()
    : impl_(dbr_pool_create())
    {
        if (!impl_)
            throw_exception();
    }

    // Copy semantics.

    Pool(const Pool&) = delete;

    Pool&
    operator =(const Pool&) = delete;

    // Move semantics.

    Pool(Pool&& rhs) noexcept
    : impl_(nullptr)
    {
        std::swap(impl_, rhs.impl_);
    }
    Pool&
    operator =(Pool&& rhs) noexcept
    {
        if (impl_) {
            dbr_pool_destroy(impl_);
            impl_ = nullptr;
        }
        std::swap(impl_, rhs.impl_);
        return *this;
    }
    void
    swap(Pool& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    operator DbrPool() noexcept
    {
        return impl_;
    }
    DbrRec*
    alloc_rec()
    {
        DbrRec* const rec = dbr_pool_alloc_rec(impl_);
        if (!rec)
            throw_exception();
        return rec;
    }
    void
    free_rec(DbrRec* rec)
    {
        dbr_pool_free_rec(impl_, rec);
    }
    DbrLevel*
    alloc_level(DbrKey key)
    {
        DbrLevel* const level = dbr_pool_alloc_level(impl_, key);
        if (!level)
            throw_exception();
        return level;
    }
    void
    free_level(DbrLevel* level)
    {
        dbr_pool_free_level(impl_, level);
    }
    DbrMatch*
    alloc_match()
    {
        DbrMatch* const match = dbr_pool_alloc_match(impl_);
        if (!match)
            throw_exception();
        return match;
    }
    void
    free_match(DbrMatch* match)
    {
        dbr_pool_free_match(impl_, match);
    }
    DbrOrder*
    alloc_order(DbrKey key)
    {
        DbrOrder* const order = dbr_pool_alloc_order(impl_, key);
        if (!order)
            throw_exception();
        return order;
    }
    void
    free_order(DbrOrder* order)
    {
        dbr_pool_free_order(impl_, order);
    }
    DbrMemb*
    alloc_memb(DbrKey key)
    {
        DbrMemb* const memb = dbr_pool_alloc_memb(impl_, key);
        if (!memb)
            throw_exception();
        return memb;
    }
    void
    free_memb(DbrMemb* memb)
    {
        dbr_pool_free_memb(impl_, memb);
    }
    DbrTrade*
    alloc_trade(DbrKey key)
    {
        DbrTrade* const trade = dbr_pool_alloc_trade(impl_, key);
        if (!trade)
            throw_exception();
        return trade;
    }
    void
    free_trade(DbrTrade* trade)
    {
        dbr_pool_free_trade(impl_, trade);
    }
    DbrPosn*
    alloc_posn(DbrKey key)
    {
        DbrPosn* const posn = dbr_pool_alloc_posn(impl_, key);
        if (!posn)
            throw_exception();
        return posn;
    }
    void
    free_posn(DbrPosn* posn)
    {
        dbr_pool_free_posn(impl_, posn);
    }
    DbrSub*
    alloc_sub(DbrKey key)
    {
        DbrSub* const sub = dbr_pool_alloc_sub(impl_, key);
        if (!sub)
            throw_exception();
        return sub;
    }
    void
    free_sub(DbrSub* sub)
    {
        dbr_pool_free_sub(impl_, sub);
    }
};

} // dbr

#endif // DBR_POOL_HPP
