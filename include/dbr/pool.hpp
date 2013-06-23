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
        return dbr_pool_alloc_rec(impl_);
    }
    void
    free_rec(DbrRec* rec)
    {
        return dbr_pool_free_rec(impl_, rec);
    }
    DbrLevel*
    alloc_level(DbrKey key)
    {
        return dbr_pool_alloc_level(impl_, key);
    }
    void
    free_level(DbrLevel* level)
    {
        return dbr_pool_free_level(impl_, level);
    }
    DbrMatch*
    alloc_match()
    {
        return dbr_pool_alloc_match(impl_);
    }
    void
    free_match(DbrMatch* match)
    {
        return dbr_pool_free_match(impl_, match);
    }
    DbrOrder*
    alloc_order(DbrKey key)
    {
        return dbr_pool_alloc_order(impl_, key);
    }
    void
    free_order(DbrOrder* order)
    {
        return dbr_pool_free_order(impl_, order);
    }
    DbrMemb*
    alloc_memb(DbrKey key)
    {
        return dbr_pool_alloc_memb(impl_, key);
    }
    void
    free_memb(DbrMemb* memb)
    {
        return dbr_pool_free_memb(impl_, memb);
    }
    DbrTrade*
    alloc_trade(DbrKey key)
    {
        return dbr_pool_alloc_trade(impl_, key);
    }
    void
    free_trade(DbrTrade* trade)
    {
        return dbr_pool_free_trade(impl_, trade);
    }
    DbrPosn*
    alloc_posn(DbrKey key)
    {
        return dbr_pool_alloc_posn(impl_, key);
    }
    void
    free_posn(DbrPosn* posn)
    {
        return dbr_pool_free_posn(impl_, posn);
    }
    DbrSub*
    alloc_sub(DbrKey key)
    {
        return dbr_pool_alloc_sub(impl_, key);
    }
    void
    free_sub(DbrSub* sub)
    {
        return dbr_pool_free_sub(impl_, sub);
    }
};

} // dbr

#endif // DBR_POOL_HPP
