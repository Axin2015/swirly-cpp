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
#ifndef SWIRLY_FIR_REST_HPP
#define SWIRLY_FIR_REST_HPP

#include <swirly/fir/EntitySet.hpp>

#include <swirly/fig/Serv.hpp>

namespace swirly {

/**
 * @addtogroup Rest
 * @{
 */

class SWIRLY_API Rest {
 public:
  Rest(Journ& journ, size_t capacity) : serv_{journ, capacity} {}
  ~Rest() noexcept;

  // Copy.
  Rest(const Rest&) = delete;
  Rest& operator=(const Rest&) = delete;

  // Move.
  Rest(Rest&&);
  Rest& operator=(Rest&&);

  void load(const Model& model, Millis now) { serv_.load(model, now); }

  void getRec(EntitySet es, Millis now, std::ostream& out) const;

  void getAsset(Millis now, std::ostream& out) const;

  void getAsset(Mnem mnem, Millis now, std::ostream& out) const;

  void getContr(Millis now, std::ostream& out) const;

  void getContr(Mnem mnem, Millis now, std::ostream& out) const;

  void getMarket(Millis now, std::ostream& out) const;

  void getMarket(Mnem mnem, Millis now, std::ostream& out) const;

  void getTrader(Millis now, std::ostream& out) const;

  void getTrader(Mnem mnem, Millis now, std::ostream& out) const;

  void getSess(Mnem trader, EntitySet es, Millis now, std::ostream& out) const;

  void getOrder(Mnem trader, Millis now, std::ostream& out) const;

  void getOrder(Mnem trader, Mnem market, Millis now, std::ostream& out) const;

  void getOrder(Mnem trader, Mnem market, Iden id, Millis now, std::ostream& out) const;

  void getTrade(Mnem trader, Millis now, std::ostream& out) const;

  void getTrade(Mnem trader, Mnem market, Millis now, std::ostream& out) const;

  void getTrade(Mnem trader, Mnem market, Iden id, Millis now, std::ostream& out) const;

  void getPosn(Mnem trader, Millis now, std::ostream& out) const;

  void getPosn(Mnem trader, Mnem contr, Millis now, std::ostream& out) const;

  void getPosn(Mnem trader, Mnem contr, IsoDate settlDate, Millis now, std::ostream& out) const;

  void getView(Millis now, std::ostream& out) const;

  void getView(ArrayView<Mnem> markets, Millis now, std::ostream& out) const;

  void postMarket(Mnem mnem, std::string_view display, Mnem contr, IsoDate settlDate,
                  IsoDate expiryDate, MarketState state, Millis now, std::ostream& out);

  void putMarket(Mnem mnem, std::optional<std::string_view> display,
                 std::optional<MarketState> state, Millis now, std::ostream& out);

  void postTrader(Mnem mnem, std::string_view display, std::string_view email, Millis now,
                  std::ostream& out);

  void putTrader(Mnem mnem, std::string_view display, Millis now, std::ostream& out);

  void postOrder(Mnem trader, Mnem market, std::string_view ref, Side side, Lots lots, Ticks ticks,
                 Lots minLots, Millis now, std::ostream& out);

  void putOrder(Mnem trader, Mnem market, ArrayView<Iden> ids, Lots lots, Millis now,
                std::ostream& out);

  void deleteOrder(Mnem market, ArrayView<Iden> ids, Millis now, std::ostream& out);

  void postTrade(Mnem market, Millis now, std::ostream& out);

  void deleteTrade(Mnem market, ArrayView<Iden> ids, Millis now, std::ostream& out);

 private:
  Serv serv_;
};

/** @} */

} // swirly

#endif // SWIRLY_FIR_REST_HPP
