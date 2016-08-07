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
#include <swirly/elm/MarketBook.hpp>

#include <swirly/ash/Array.hpp>

#include <experimental/optional>

namespace std {
template <typename T>
using optional = experimental::optional<T>;
using experimental::nullopt;
}

namespace swirly {

class Accnt;
class Journ;
class MarketBook;
class Model;
class Response;

using TradePair = std::pair<ExecPtr, ExecPtr>;

class SWIRLY_API Serv {
 public:
  Serv(Journ& journ, std::size_t capacity);

  ~Serv() noexcept;

  // Copy.
  Serv(const Serv&) = delete;
  Serv& operator=(const Serv&) = delete;

  // Move.
  Serv(Serv&&);
  Serv& operator=(Serv&&);

  void load(const Model& model, Millis now);

  AssetSet& assets() const noexcept;

  ContrSet& contrs() const noexcept;

  MarketSet& markets() const noexcept;

  MarketBook& market(Mnem mnem) const;

  Accnt& accnt(Mnem mnem) const;

  MarketBook& createMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay,
                           Jday expiryDay, MarketState state, Millis now);

  MarketBook& updateMarket(Mnem mnem, std::optional<std::string_view> display,
                           std::optional<MarketState> state, Millis now);

  void createOrder(Accnt& accnt, MarketBook& book, std::string_view ref, Side side, Lots lots,
                   Ticks ticks, Lots minLots, Millis now, Response& resp);

  void reviseOrder(Accnt& accnt, MarketBook& book, Order& order, Lots lots, Millis now,
                   Response& resp);

  void reviseOrder(Accnt& accnt, MarketBook& book, Iden id, Lots lots, Millis now, Response& resp);

  void reviseOrder(Accnt& accnt, MarketBook& book, std::string_view ref, Lots lots, Millis now,
                   Response& resp);

  void reviseOrder(Accnt& accnt, MarketBook& book, ArrayView<Iden> ids, Lots lots, Millis now,
                   Response& resp);

  void cancelOrder(Accnt& accnt, MarketBook& book, Order& order, Millis now, Response& resp);

  void cancelOrder(Accnt& accnt, MarketBook& book, Iden id, Millis now, Response& resp);

  void cancelOrder(Accnt& accnt, MarketBook& book, std::string_view ref, Millis now,
                   Response& resp);

  void cancelOrder(Accnt& accnt, MarketBook& book, ArrayView<Iden> ids, Millis now, Response& resp);

  /**
   * Cancels all orders.
   *
   * @param accnt
   *            The account.
   * @param now
   *            The current time.
   */
  void cancelOrder(Accnt& accnt, Millis now);

  void cancelOrder(MarketBook& book, Millis now);

  TradePair createTrade(Accnt& accnt, MarketBook& book, std::string_view ref, Side side, Lots lots,
                        Ticks ticks, LiqInd liqInd, Mnem cpty, Millis created);

  void archiveTrade(Accnt& accnt, const Exec& trade, Millis now);

  void archiveTrade(Accnt& accnt, Mnem market, Iden id, Millis now);

  void archiveTrade(Accnt& accnt, Mnem market, ArrayView<Iden> ids, Millis now);

  /**
   * This method may partially fail.
   *
   * @param now
   *            The current time.
   */
  void expireEndOfDay(Millis now);

  void settlEndOfDay(Millis now);

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

} // swirly

#endif // SWIRLY_FIG_SERV_HPP
