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
#ifndef SWIRLY_LOB_SERV_HPP
#define SWIRLY_LOB_SERV_HPP

#include <swirly/fin/Asset.hpp>
#include <swirly/fin/Instr.hpp>
#include <swirly/fin/Market.hpp>

#include <swirly/util/Array.hpp>

namespace swirly {

inline namespace app {
template <typename ValueT>
class MemQueue;
} // namespace app

inline namespace fin {
class Journ;
class Market;
class Model;
class MsgQueue;
} // namespace fin

inline namespace lob {

class Accnt;
class Response;

using TradePair = std::pair<ConstExecPtr, ConstExecPtr>;

class SWIRLY_API Serv {
  public:
    Serv(MsgQueue& mq, std::size_t maxExecs);

    ~Serv();

    // Copy.
    Serv(const Serv&) = delete;
    Serv& operator=(const Serv&) = delete;

    // Move.
    Serv(Serv&&);
    Serv& operator=(Serv&&);

    void load(const Model& model, Time now);

    const AssetSet& assets() const noexcept;

    const Instr& instr(Symbol symbol) const;

    const InstrSet& instrs() const noexcept;

    const Accnt& accnt(Symbol symbol) const;

    const Market& market(Id64 id) const;

    const MarketSet& markets() const noexcept;

    const Market& createMarket(const Instr& instr, JDay settlDay, MarketState state, Time now);

    void updateMarket(const Market& market, MarketState state, Time now);

    void createOrder(const Accnt& accnt, const Market& market, std::string_view ref, Side side,
                     Lots lots, Ticks ticks, Lots minLots, Time now, Response& resp);

    void reviseOrder(const Accnt& accnt, const Market& market, const Order& order, Lots lots,
                     Time now, Response& resp);

    void reviseOrder(const Accnt& accnt, const Market& market, Id64 id, Lots lots, Time now,
                     Response& resp);

    void reviseOrder(const Accnt& accnt, const Market& market, std::string_view ref, Lots lots,
                     Time now, Response& resp);

    void reviseOrder(const Accnt& accnt, const Market& market, ArrayView<Id64> ids, Lots lots,
                     Time now, Response& resp);

    void cancelOrder(const Accnt& accnt, const Market& market, const Order& order, Time now,
                     Response& resp);

    void cancelOrder(const Accnt& accnt, const Market& market, Id64 id, Time now, Response& resp);

    void cancelOrder(const Accnt& accnt, const Market& market, std::string_view ref, Time now,
                     Response& resp);

    void cancelOrder(const Accnt& accnt, const Market& market, ArrayView<Id64> ids, Time now,
                     Response& resp);

    /**
     * Cancels all orders.
     *
     * @param accnt
     *            The account.
     * @param now
     *            The current time.
     */
    void cancelOrder(const Accnt& accnt, Time now);

    void cancelOrder(const Market& market, Time now);

    TradePair createTrade(const Accnt& accnt, const Market& market, std::string_view ref, Side side,
                          Lots lots, Ticks ticks, LiqInd liqInd, Symbol cpty, Time created);

    void archiveTrade(const Accnt& accnt, const Exec& trade, Time now);

    void archiveTrade(const Accnt& accnt, Id64 marketId, Id64 id, Time now);

    void archiveTrade(const Accnt& accnt, Id64 marketId, ArrayView<Id64> ids, Time now);

    /**
     * This method may partially fail.
     *
     * @param now
     *            The current time.
     */
    void expireEndOfDay(Time now);

    void settlEndOfDay(Time now);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace lob
} // namespace swirly

#endif // SWIRLY_LOB_SERV_HPP
