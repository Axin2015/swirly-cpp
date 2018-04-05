/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#ifndef SWIRLY_FIN_MARKET_HPP
#define SWIRLY_FIN_MARKET_HPP

#include <swirly/fin/MarketSide.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/Set.hpp>

namespace swirly {
inline namespace fin {

using MarketPtr = boost::intrusive_ptr<Market>;
using ConstMarketPtr = boost::intrusive_ptr<const Market>;

class SWIRLY_API Market
: public RefCount<Market, ThreadUnsafePolicy>
, public Comparable<Market> {
  public:
    Market(Id64 id, Symbol broker, Symbol instr, JDay settlDay, MarketState state,
           Lots lastLots = 0_lts, Ticks lastTicks = 0_tks, Time lastTime = {},
           Id64 maxId = 0_id64) noexcept
    : id_{id}
    , broker_{broker}
    , instr_{instr}
    , settlDay_{settlDay}
    , state_{state}
    , lastLots_{lastLots}
    , lastTicks_{lastTicks}
    , lastTime_{lastTime}
    , maxId_{maxId}
    {
    }
    ~Market();

    // Copy.
    Market(const Market&) = delete;
    Market& operator=(const Market&) = delete;

    // Move.
    Market(Market&&);
    Market& operator=(Market&&) = delete;

    template <typename... ArgsT>
    static MarketPtr make(ArgsT&&... args)
    {
        return makeIntrusive<Market>(std::forward<ArgsT>(args)...);
    }

    void toDsv(std::ostream& os, char sep = ',') const;
    void toJson(std::ostream& os) const;

    int compare(const Market& rhs) const noexcept { return swirly::compare(id_, rhs.id_); }
    auto id() const noexcept { return id_; }
    auto broker() const noexcept { return broker_; }
    auto instr() const noexcept { return instr_; }
    auto settlDay() const noexcept { return settlDay_; }
    auto state() const noexcept { return state_; }
    Lots lastLots() const noexcept { return lastLots_; }
    Ticks lastTicks() const noexcept { return lastTicks_; }
    Time lastTime() const noexcept { return lastTime_; }
    const MarketSide& bidSide() const noexcept { return bidSide_; }
    const MarketSide& offerSide() const noexcept { return offerSide_; }
    Id64 maxId() const noexcept { return maxId_; }

    void setState(MarketState state) noexcept { state_ = state; }
    MarketSide& bidSide() noexcept { return bidSide_; }
    MarketSide& offerSide() noexcept { return offerSide_; }
    /**
     * Throws std::bad_alloc.
     */
    void insertOrder(const OrderPtr& order) { side(order->side()).insertOrder(order); }
    void removeOrder(const Order& order) noexcept { side(order.side()).removeOrder(order); }
    /**
     * Throws std::bad_alloc.
     */
    void createOrder(const OrderPtr& order, Time now)
    {
        side(order->side()).createOrder(order, now);
    }
    void reviseOrder(Order& order, Lots lots, Time now) noexcept
    {
        side(order.side()).reviseOrder(order, lots, now);
    }
    void cancelOrder(Order& order, Time now) noexcept
    {
        side(order.side()).cancelOrder(order, now);
    }
    void takeOrder(Order& order, Lots lots, Time now) noexcept
    {
        side(order.side()).takeOrder(order, lots, now);
        lastLots_ = lots;
        lastTicks_ = order.ticks();
        lastTime_ = now;
    }
    Id64 allocId() noexcept { return ++maxId_; }
    boost::intrusive::set_member_hook<> idHook;

  private:
    MarketSide& side(Side side) noexcept { return side == Side::Buy ? bidSide_ : offerSide_; }
    const MarketSide& side(Side side) const noexcept
    {
        return side == Side::Buy ? bidSide_ : offerSide_;
    }

    const Id64 id_;
    const Symbol broker_;
    const Symbol instr_;
    const JDay settlDay_;
    MarketState state_;
    Lots lastLots_;
    Ticks lastTicks_;
    Time lastTime_;
    // Two sides constitute the market.
    MarketSide bidSide_;
    MarketSide offerSide_;
    Id64 maxId_;
};

inline std::ostream& operator<<(std::ostream& os, const Market& market)
{
    market.toJson(os);
    return os;
}

using MarketSet = IdSet<Market>;

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_MARKET_HPP
