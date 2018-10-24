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
#ifndef SWIRLY_LOB_APP_HPP
#define SWIRLY_LOB_APP_HPP

#include <swirly/lob/Types.hpp>

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

class Response;
class Sess;

using TradePair = std::pair<ConstExecPtr, ConstExecPtr>;

class SWIRLY_API LobApp {
  public:
    LobApp(MsgQueue& mq, std::size_t max_execs);

    ~LobApp();

    // Copy.
    LobApp(const LobApp&) = delete;
    LobApp& operator=(const LobApp&) = delete;

    // Move.
    LobApp(LobApp&&);
    LobApp& operator=(LobApp&&);

    const AssetSet& assets() const noexcept;

    const InstrSet& instrs() const noexcept;

    const MarketSet& markets() const noexcept;

    const Instr& instr(Symbol symbol) const;

    const Market& market(Id64 id) const;

    const Sess& sess(Symbol accnt) const;

    void load(CyclTime now, const Model& model);

    void set_trade_slot(const Sess& sess, TradeSlot slot) noexcept;

    const Market& create_market(CyclTime now, Id64 id, const Instr& instr, JDay settl_day,
                                MarketState state);

    void update_market(CyclTime now, const Market& market, MarketState state);

    OrderPtr create_order(CyclTime now, const Sess& sess, const Market& market,
                          std::string_view ref, Side side, Lots lots, Ticks ticks, Lots min_lots,
                          Response& resp);

    OrderPtr create_quote(CyclTime now, const Sess& sess, const Market& market,
                          std::string_view ref, Side side, Lots lots, Ticks ticks, Lots min_lots);

    void revise_order(CyclTime now, const Sess& sess, const Market& market, const Order& order,
                      Lots lots, Response& resp);

    void revise_order(CyclTime now, const Sess& sess, const Market& market, Id64 id, Lots lots,
                      Response& resp);

    void revise_order(CyclTime now, const Sess& sess, const Market& market, std::string_view ref,
                      Lots lots, Response& resp);

    void revise_order(CyclTime now, const Sess& sess, const Market& market, ArrayView<Id64> ids,
                      Lots lots, Response& resp);

    void cancel_order(CyclTime now, const Sess& sess, const Market& market, const Order& order,
                      Response& resp);

    void cancel_order(CyclTime now, const Sess& sess, const Market& market, Id64 id,
                      Response& resp);

    void cancel_order(CyclTime now, const Sess& sess, const Market& market, std::string_view ref,
                      Response& resp);

    void cancel_order(CyclTime now, const Sess& sess, const Market& market, ArrayView<Id64> ids,
                      Response& resp);

    /**
     * Cancels all orders.
     *
     * @param accnt
     *            The account.
     * @param now
     *            The current time.
     */
    void cancel_order(CyclTime now, const Sess& sess);

    void cancel_order(CyclTime now, const Market& market);

    bool try_cancel_order(CyclTime now, const Sess& sess, const Market& market, const Order& order,
                          Response& resp);

    bool try_cancel_order(CyclTime now, const Sess& sess, const Market& market, Id64 id,
                          Response& resp);

    bool try_cancel_order(CyclTime now, const Sess& sess, const Market& market,
                          std::string_view ref, Response& resp);

    bool try_cancel_quote(CyclTime now, const Sess& sess, const Market& market,
                          const Order& order) noexcept;

    TradePair create_trade(CyclTime now, const Sess& sess, const Market& market,
                           std::string_view ref, Side side, Lots lots, Ticks ticks, LiqInd liq_ind,
                           Symbol cpty);

    void archive_trade(CyclTime now, const Sess& sess, const Exec& trade);

    void archive_trade(CyclTime now, const Sess& sess, Id64 market_id, Id64 id);

    void archive_trade(CyclTime now, const Sess& sess, Id64 market_id, ArrayView<Id64> ids);

    /**
     * This method may partially fail.
     *
     * @param now
     *            The current time.
     */
    void expire_end_of_day(CyclTime now);

    void settl_end_of_day(CyclTime now);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace lob
} // namespace swirly

#endif // SWIRLY_LOB_APP_HPP
