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
#ifndef DBRPP_POOL_HPP
#define DBRPP_POOL_HPP

#include <dbrpp/except.hpp>

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
    Pool(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    Pool()
        : impl_{dbr_pool_create()}
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrPool() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Pool(const Pool&) = delete;

    Pool&
    operator =(const Pool&) = delete;

    // Move semantics.

    Pool(Pool&& rhs) noexcept
        : impl_{nullptr}
    {
        swap(rhs);
    }
    Pool&
    operator =(Pool&& rhs) noexcept
    {
        if (impl_) {
            dbr_pool_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(Pool& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
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
    free_rec(DbrRec* rec) noexcept
    {
        dbr_pool_free_rec(impl_, rec);
    }
    DbrLevel*
    alloc_level()
    {
        DbrLevel* const level = dbr_pool_alloc_level(impl_);
        if (!level)
            throw_exception();
        return level;
    }
    void
    free_level(DbrLevel* level) noexcept
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
    free_match(DbrMatch* match) noexcept
    {
        dbr_pool_free_match(impl_, match);
    }
    DbrOrder*
    alloc_order()
    {
        DbrOrder* const order = dbr_pool_alloc_order(impl_);
        if (!order)
            throw_exception();
        return order;
    }
    void
    free_order(DbrOrder* order) noexcept
    {
        dbr_pool_free_order(impl_, order);
    }
    DbrExec*
    alloc_exec()
    {
        DbrExec* const exec = dbr_pool_alloc_exec(impl_);
        if (!exec)
            throw_exception();
        return exec;
    }
    void
    free_exec(DbrExec* exec) noexcept
    {
        dbr_pool_free_exec(impl_, exec);
    }
    DbrMemb*
    alloc_memb()
    {
        DbrMemb* const memb = dbr_pool_alloc_memb(impl_);
        if (!memb)
            throw_exception();
        return memb;
    }
    void
    free_memb(DbrMemb* memb) noexcept
    {
        dbr_pool_free_memb(impl_, memb);
    }
    DbrPosn*
    alloc_posn()
    {
        DbrPosn* const posn = dbr_pool_alloc_posn(impl_);
        if (!posn)
            throw_exception();
        return posn;
    }
    void
    free_posn(DbrPosn* posn) noexcept
    {
        dbr_pool_free_posn(impl_, posn);
    }
};

} // dbr

#endif // DBRPP_POOL_HPP
