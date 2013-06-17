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
};

} // dbr

#endif // DBR_POOL_HPP
