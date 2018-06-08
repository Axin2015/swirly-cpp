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
    Serv(MsgQueue& mq, std::size_t max_execs);

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

    const Market& create_market(const Instr& instr, JDay settl_day, MarketState state, Time now);

    void update_market(const Market& market, MarketState state, Time now);

    void create_order(const Accnt& accnt, const Market& market, std::string_view ref, Side side,
                      Lots lots, Ticks ticks, Lots min_lots, Time now, Response& resp);

    void revise_order(const Accnt& accnt, const Market& market, const Order& order, Lots lots,
                      Time now, Response& resp);

    void revise_order(const Accnt& accnt, const Market& market, Id64 id, Lots lots, Time now,
                      Response& resp);

    void revise_order(const Accnt& accnt, const Market& market, std::string_view ref, Lots lots,
                      Time now, Response& resp);

    void revise_order(const Accnt& accnt, const Market& market, ArrayView<Id64> ids, Lots lots,
                      Time now, Response& resp);

    void cancel_order(const Accnt& accnt, const Market& market, const Order& order, Time now,
                      Response& resp);

    void cancel_order(const Accnt& accnt, const Market& market, Id64 id, Time now, Response& resp);

    void cancel_order(const Accnt& accnt, const Market& market, std::string_view ref, Time now,
                      Response& resp);

    void cancel_order(const Accnt& accnt, const Market& market, ArrayView<Id64> ids, Time now,
                      Response& resp);

    /**
     * Cancels all orders.
     *
     * @param accnt
     *            The account.
     * @param now
     *            The current time.
     */
    void cancel_order(const Accnt& accnt, Time now);

    void cancel_order(const Market& market, Time now);

    TradePair create_trade(const Accnt& accnt, const Market& market, std::string_view ref,
                           Side side, Lots lots, Ticks ticks, LiqInd liq_ind, Symbol cpty,
                           Time created);

    void archive_trade(const Accnt& accnt, const Exec& trade, Time now);

    void archive_trade(const Accnt& accnt, Id64 market_id, Id64 id, Time now);

    void archive_trade(const Accnt& accnt, Id64 market_id, ArrayView<Id64> ids, Time now);

    /**
     * This method may partially fail.
     *
     * @param now
     *            The current time.
     */
    void expire_end_of_day(Time now);

    void settl_end_of_day(Time now);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace lob
} // namespace swirly

#endif // SWIRLY_LOB_SERV_HPP
