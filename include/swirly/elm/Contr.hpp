/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
 public:
    Contr(const StringView& mnem, const StringView& display, const StringView& asset,
          const StringView& ccy, int lotNumer, int lotDenom, int tickNumer, int tickDenom,
          int pipDp, Lots minLots, Lots maxLots) noexcept;

    ~Contr() noexcept override;

    // Copy.
    Contr(const Contr&);
    Contr& operator=(const Contr&) = delete;

    // Move.
    Contr(Contr&&);
    Contr& operator=(Contr&&) = delete;

    auto asset() const noexcept
    {
        return +asset_;
    }
    auto ccy() const noexcept
    {
        return +ccy_;
    }
    auto lotNumer() const noexcept
    {
        return lotNumer_;
    }
    auto lotDenom() const noexcept
    {
        return lotDenom_;
    }
    auto qtyInc() const noexcept
    {
        return qtyInc_;
    }
    auto tickNumer() const noexcept
    {
        return tickNumer_;
    }
    auto tickDenom() const noexcept
    {
        return tickDenom_;
    }
    auto priceInc() const noexcept
    {
        return priceInc_;
    }
    auto pipDp() const noexcept
    {
        return pipDp_;
    }
    auto qtyDp() const noexcept
    {
        return qtyDp_;
    }
    auto priceDp() const noexcept
    {
        return priceDp_;
    }
    auto minLots() const noexcept
    {
        return minLots_;
    }
    auto maxLots() const noexcept
    {
        return maxLots_;
    }
    boost::intrusive::set_member_hook<> mnemHook_;

 private:
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
};

using ContrSet = RecSet<Contr>;

/** @} */

} // swirly

#endif // SWIRLY_ELM_CONTR_HPP
