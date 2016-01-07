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
#include <swirly/ash/Types.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API Posn : public RefCounted {

    const Mnem trader_;
    const Mnem contr_;
    Jd settlDay_;
    Lots buyLots_;
    Cost buyCost_;
    Lots sellLots_;
    Cost sellCost_;

 public:
    Posn(const StringView& trader, const StringView& contr, Jd settlDay,
         Lots buyLots, Cost buyCost, Lots sellLots, Cost sellCost) noexcept
    :   trader_{trader},
        contr_{contr},
        settlDay_{settlDay},
        buyLots_{buyLots},
        buyCost_{buyCost},
        sellLots_{sellLots},
        sellCost_{sellCost}
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
    StringView contr() const noexcept
    {
        return contr_.view();
    }
    Jd settlDay() const noexcept
    {
        return settlDay_;
    }
    Lots buyLots() const noexcept
    {
        return buyLots_;
    }
    Cost buyCost() const noexcept
    {
        return buyCost_;
    }
    Lots sellLots() const noexcept
    {
        return sellLots_;
    }
    Cost sellCost() const noexcept
    {
        return sellCost_;
    }
};

using PosnPtr = boost::intrusive_ptr<Posn>;

/** @} */

} // swirly

#endif // SWIRLY_ELM_POSN_HPP
