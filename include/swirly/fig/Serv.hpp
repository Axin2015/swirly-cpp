/*
 * The Restful Matching-Engine.
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
#ifndef SWIRLY_FIG_SERV_HPP
#define SWIRLY_FIG_SERV_HPP

#include <swirly/elm/Asset.hpp>
#include <swirly/elm/Contr.hpp>
#include <swirly/elm/Market.hpp>
#include <swirly/elm/Trader.hpp>

#include <swirly/ash/Array.hpp>

#include <experimental/optional>

namespace std {
template <typename T>
using optional = experimental::optional<T>;
using experimental::nullopt;
}

/**
 * @addtogroup App
 * @{
 */

namespace swirly {

class Journ;
class MarketBook;
class Model;
class Response;
class TraderSess;

class SWIRLY_API Serv {
 public:
  Serv(const Model& model, Journ& journ, Millis now);

  ~Serv() noexcept;

  // Copy.
  Serv(const Serv&) = delete;
  Serv& operator=(const Serv&) = delete;

  // Move.
  Serv(Serv&&);
  Serv& operator=(Serv&&);

  const AssetSet& assets() const noexcept;

  const ContrSet& contrs() const noexcept;

  const MarketSet& markets() const noexcept;

  const TraderSet& traders() const noexcept;

  MarketBook& market(std::string_view mnem) const;

  TraderSess& trader(std::string_view mnem) const;

  TraderSess* findTraderByEmail(std::string_view email) const;

  const MarketBook& createMarket(std::string_view mnem, std::string_view display,
                                 std::string_view contr, Jday settlDay, Jday expiryDay,
                                 MarketState state, Millis now);

  const MarketBook& updateMarket(std::string_view mnem, std::optional<std::string_view> display,
                                 std::optional<MarketState> state, Millis now);

  const TraderSess& createTrader(std::string_view mnem, std::string_view display,
                                 std::string_view email, Millis now);

  const TraderSess& updateTrader(std::string_view mnem, std::string_view display, Millis now);

  void createOrder(TraderSess& sess, MarketBook& book, std::string_view ref, Side side, Lots lots,
                   Ticks ticks, Lots minLots, Millis now, Response& resp);

  void reviseOrder(TraderSess& sess, MarketBook& book, Order& order, Lots lots, Millis now,
                   Response& resp);

  void reviseOrder(TraderSess& sess, MarketBook& book, Iden id, Lots lots, Millis now,
                   Response& resp);

  void reviseOrder(TraderSess& sess, MarketBook& book, std::string_view ref, Lots lots, Millis now,
                   Response& resp);

  void reviseOrder(TraderSess& sess, MarketBook& book, ArrayView<Iden> ids, Lots lots, Millis now,
                   Response& resp);

  void cancelOrder(TraderSess& sess, MarketBook& book, Order& order, Millis now, Response& resp);

  void cancelOrder(TraderSess& sess, MarketBook& book, Iden id, Millis now, Response& resp);

  void cancelOrder(TraderSess& sess, MarketBook& book, std::string_view ref, Millis now,
                   Response& resp);

  void cancelOrder(TraderSess& sess, MarketBook& book, ArrayView<Iden> ids, Millis now,
                   Response& resp);

  /**
   * Cancels all orders.
   *
   * @param sess
   *            The session.
   * @param now
   *            The current time.
   */
  void cancelOrder(TraderSess& sess, Millis now);

  void cancelOrder(MarketBook& book, Millis now);

  void archiveOrder(TraderSess& sess, Order& order, Millis now);

  void archiveOrder(TraderSess& sess, std::string_view market, Iden id, Millis now);

  /**
   * Archive all orders.
   *
   * @param sess
   *            The session.
   * @param now
   *            The current time.
   */
  void archiveOrder(TraderSess& sess, Millis now);

  void archiveOrder(TraderSess& sess, std::string_view market, ArrayView<Iden> ids, Millis now);

  ConstExecPtr createTrade(TraderSess& sess, MarketBook& book, std::string_view ref, Side side,
                           Lots lots, Ticks ticks, Role role, std::string_view cpty,
                           Millis created);

  void archiveTrade(TraderSess& sess, Exec& trade, Millis now);

  void archiveTrade(TraderSess& sess, std::string_view market, Iden id, Millis now);

  /**
   * Archive all trades.
   *
   * @param sess
   *            The session.
   * @param now
   *            The current time.
   */
  void archiveTrade(TraderSess& sess, Millis now);

  void archiveTrade(TraderSess& sess, std::string_view market, ArrayView<Iden> ids, Millis now);

  /**
   * This method may partially fail.
   *
   * @param now
   *            The current time.
   */
  void expireEndOfDay(Millis now);

  void settlEndOfDay(Millis now);

  void poll(Millis now);

  Millis getTimeout() const noexcept;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // swirly

/** @} */

#endif // SWIRLY_FIG_SERV_HPP
