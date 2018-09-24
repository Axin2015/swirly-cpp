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
    Market(Id64 id, Symbol instr, JDay settl_day, MarketState state, WallTime last_time = {},
           Lots last_lots = 0_lts, Ticks last_ticks = 0_tks, Id64 max_id = 0_id64) noexcept
    : id_{id}
    , instr_{instr}
    , settl_day_{settl_day}
    , state_{state}
    , last_time_{last_time}
    , last_lots_{last_lots}
    , last_ticks_{last_ticks}
    , max_id_{max_id}
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
        return make_intrusive<Market>(std::forward<ArgsT>(args)...);
    }

    void to_dsv(std::ostream& os, char delim = ',') const;
    void to_json(std::ostream& os) const;

    int compare(const Market& rhs) const noexcept { return swirly::compare(id_, rhs.id_); }
    auto id() const noexcept { return id_; }
    auto instr() const noexcept { return instr_; }
    auto settl_day() const noexcept { return settl_day_; }
    auto state() const noexcept { return state_; }
    WallTime last_time() const noexcept { return last_time_; }
    Lots last_lots() const noexcept { return last_lots_; }
    Ticks last_ticks() const noexcept { return last_ticks_; }
    const MarketSide& bid_side() const noexcept { return bid_side_; }
    const MarketSide& offer_side() const noexcept { return offer_side_; }
    Id64 max_id() const noexcept { return max_id_; }

    void set_state(MarketState state) noexcept { state_ = state; }
    MarketSide& bid_side() noexcept { return bid_side_; }
    MarketSide& offer_side() noexcept { return offer_side_; }
    /**
     * Throws std::bad_alloc.
     */
    void insert_order(const OrderPtr& order) { side(order->side()).insert_order(order); }
    void remove_order(const Order& order) noexcept { side(order.side()).remove_order(order); }
    /**
     * Throws std::bad_alloc.
     */
    void create_order(WallTime now, const OrderPtr& order)
    {
        side(order->side()).create_order(now, order);
    }
    void revise_order(WallTime now, Order& order, Lots lots) noexcept
    {
        side(order.side()).revise_order(now, order, lots);
    }
    void cancel_order(WallTime now, Order& order) noexcept
    {
        side(order.side()).cancel_order(now, order);
    }
    void take_order(WallTime now, Order& order, Lots lots) noexcept
    {
        side(order.side()).take_order(now, order, lots);
        last_time_ = now;
        last_lots_ = lots;
        last_ticks_ = order.ticks();
    }
    Id64 alloc_id() noexcept { return ++max_id_; }
    boost::intrusive::set_member_hook<> id_hook;

  private:
    MarketSide& side(Side side) noexcept { return side == Side::Buy ? bid_side_ : offer_side_; }
    const MarketSide& side(Side side) const noexcept
    {
        return side == Side::Buy ? bid_side_ : offer_side_;
    }

    const Id64 id_;
    const Symbol instr_;
    const JDay settl_day_;
    MarketState state_;
    WallTime last_time_;
    Lots last_lots_;
    Ticks last_ticks_;
    // Two sides constitute the market.
    MarketSide bid_side_;
    MarketSide offer_side_;
    Id64 max_id_;
};

inline std::ostream& operator<<(std::ostream& os, const Market& market)
{
    market.to_json(os);
    return os;
}

using MarketSet = IdSet<Market>;

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_MARKET_HPP
