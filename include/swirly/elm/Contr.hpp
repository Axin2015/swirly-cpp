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
#ifndef SWIRLY_ELM_CONTR_HPP
#define SWIRLY_ELM_CONTR_HPP

#include <swirly/elm/Rec.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

/**
 * A specification that stipulates the terms and conditions of sale.
 */
class SWIRLY_API Contr : public Rec {
protected:
    const Mnem asset_;
    const Mnem ccy_;
    const int lotNumer_;
    const int lotDenom_;
    // Transient.
    const double qtyInc_;
    const int tickNumer_;
    const int tickDenom_;
    // Transient.
    const double priceInc_;
    const int pipDp_;
    // Transient.
    const int qtyDp_;
    // Transient.
    const int priceDp_;
    const Lots minLots_;
    const Lots maxLots_;

public:
    Contr(const StringView& mnem, const StringView& display, const StringView& asset,
          const StringView& ccy, int lotNumer, int lotDenom, int tickNumer, int tickDenom,
          int pipDp, Lots minLots, Lots maxLots) noexcept;

    ~Contr() noexcept override;

    // Copy.
    Contr(const Contr&) = default;
    Contr& operator =(const Contr&) = default;

    // Move.
    Contr(Contr&&) = default;
    Contr& operator =(Contr&&) = default;

    StringView asset() const noexcept
    {
        return asset_.view();
    }
    StringView ccy() const noexcept
    {
        return ccy_.view();
    }
    int lotNumer() const noexcept
    {
        return lotNumer_;
    }
    int lotDenom() const noexcept
    {
        return lotDenom_;
    }
    double qtyInc() const noexcept
    {
        return qtyInc_;
    }
    int tickNumer() const noexcept
    {
        return tickNumer_;
    }
    int tickDenom() const noexcept
    {
        return tickDenom_;
    }
    double priceInc() const noexcept
    {
        return priceInc_;
    }
    int pipDp() const noexcept
    {
        return pipDp_;
    }
    int qtyDp() const noexcept
    {
        return qtyDp_;
    }
    int priceDp() const noexcept
    {
        return priceDp_;
    }
    Lots minLots() const noexcept
    {
        return minLots_;
    }
    Lots maxLots() const noexcept
    {
        return maxLots_;
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_CONTR_HPP
