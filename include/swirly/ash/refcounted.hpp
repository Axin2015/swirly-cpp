/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#ifndef SWIRLY_ASH_REFCOUNTED_HPP
#define SWIRLY_ASH_REFCOUNTED_HPP

#include <swirly/ash/defs.hpp>

#include <boost/intrusive_ptr.hpp>

namespace swirly {

class SWIRLY_API RefCounted {
    mutable int refs_ = 0;
    friend void intrusive_ptr_add_ref(const RefCounted* ptr) noexcept
    {
        ++ptr->refs_;
    }
    friend void intrusive_ptr_release(const RefCounted* ptr) noexcept
    {
        if (--ptr->refs_ <= 0) {
            delete ptr;
        }
    }
public:
    constexpr RefCounted() noexcept = default;

    virtual ~RefCounted() noexcept;

    // Copy.
    constexpr RefCounted(const RefCounted&) noexcept = default;
    RefCounted& operator =(const RefCounted&) noexcept = default;

    // Move.
    constexpr RefCounted(RefCounted&&) noexcept = default;
    RefCounted& operator =(RefCounted&&) noexcept = default;
};

using RefCountedPtr = boost::intrusive_ptr<RefCounted>;

} // swirly

#endif // SWIRLY_ASH_REFCOUNTED_HPP
