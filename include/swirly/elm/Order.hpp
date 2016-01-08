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
#ifndef SWIRLY_ELM_ORDER_HPP
#define SWIRLY_ELM_ORDER_HPP

#include <swirly/elm/Request.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

/**
 * An instruction to buy or sell goods or services.
 */
class SWIRLY_API Order : public Request {

    const Iden quoteId_;
    State state_;
    const Ticks ticks_;
    /**
     * Must be greater than zero.
     */
    Lots resd_;
    Lots quotd_{0};
    /**
     * Must not be greater that lots.
     */
    Lots exec_;
    Cost cost_;
    Lots lastLots_;
    Ticks lastTicks_;
    /**
     * Minimum to be filled by this order.
     */
    const Lots minLots_;
    bool pecan_;
    Millis modified_;

 public:
    boost::intrusive::set_member_hook<> idHook_;

    Order(const StringView& trader, const StringView& market, const StringView& contr,
          Jd settlDay, Iden id, const StringView& ref, Iden quoteId, State state, Side side,
          Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots, Ticks lastTicks,
          Lots minLots, bool pecan, Millis created, Millis modified) noexcept
    :   Request{trader, market, contr, settlDay, id, ref, side, lots, created},
        quoteId_{quoteId},
        state_{state},
        ticks_{ticks},
        resd_{resd},
        exec_{exec},
        cost_{cost},
        lastLots_{lastLots},
        lastTicks_{lastTicks},
        minLots_{minLots},
        pecan_{pecan},
        modified_{modified}
    {
    }

    ~Order() noexcept override;

    // Copy.
    Order(const Order&) = default;
    Order& operator =(const Order&) = default;

    // Move.
    Order(Order&&) = default;
    Order& operator =(Order&&) = default;

    Iden quoteId() const noexcept
    {
        return quoteId_;
    }
    State state() const noexcept
    {
        return state_;
    }
    Ticks ticks() const noexcept
    {
        return ticks_;
    }
    Lots resd() const noexcept
    {
        return resd_;
    }
    Lots quotd() const noexcept
    {
        return quotd_;
    }
    Lots exec() const noexcept
    {
        return exec_;
    }
    Cost cost() const noexcept
    {
        return cost_;
    }
    Lots lastLots() const noexcept
    {
        return lastLots_;
    }
    Ticks lastTicks() const noexcept
    {
        return lastTicks_;
    }
    Lots minLots() const noexcept
    {
        return minLots_;
    }
    bool pecan() const noexcept
    {
        return pecan_;
    }
    Millis modified() const noexcept
    {
        return modified_;
    }
};

using OrderPtr = boost::intrusive_ptr<Order>;
using OrderIdSet = RequestIdSet<Order>;

/** @} */

} // swirly

#endif // SWIRLY_ELM_ORDER_HPP
