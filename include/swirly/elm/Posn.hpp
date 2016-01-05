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
#ifndef SWIRLY_ELM_POSN_HPP
#define SWIRLY_ELM_POSN_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/RefCounted.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API Posn : public RefCounted {
 protected:
    const Mnem trader_;
 public:
    constexpr explicit Posn(const StringView& trader) noexcept
    :   trader_{trader}
    {
    }

    ~Posn() noexcept override;

    // Copy.
    constexpr Posn(const Posn&) noexcept = default;
    Posn& operator =(const Posn&) noexcept = default;

    // Move.
    constexpr Posn(Posn&&) noexcept = default;
    Posn& operator =(Posn&&) noexcept = default;

    StringView trader() const noexcept
    {
        return trader_.view();
    }
};

using PosnPtr = boost::intrusive_ptr<Posn>;

/** @} */

} // swirly

#endif // SWIRLY_ELM_POSN_HPP
