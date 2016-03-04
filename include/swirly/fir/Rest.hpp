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
  Rest(const Model& model, Journ& journ, Millis now) : serv_{model, journ, now} {}
  ~Rest() noexcept;

  // Copy.
  Rest(const Rest&) = delete;
  Rest& operator=(const Rest&) = delete;

  // Move.
  Rest(Rest&&);
  Rest& operator=(Rest&&);

  void getRec(EntitySet es, Millis now, std::ostream& out) const;

  void getAsset(Millis now, std::ostream& out) const;

  void getAsset(std::string_view mnem, Millis now, std::ostream& out) const;

  void getContr(Millis now, std::ostream& out) const;

  void getContr(std::string_view mnem, Millis now, std::ostream& out) const;

  void getMarket(Millis now, std::ostream& out) const;

  void postMarket(Millis now, std::ostream& out) const;

  void getMarket(std::string_view mnem, Millis now, std::ostream& out) const;

  void putMarket(std::string_view mnem, Millis now, std::ostream& out) const;

  void getTrader(Millis now, std::ostream& out) const;

  void postTrader(Millis now, std::ostream& out) const;

  void getTrader(std::string_view mnem, Millis now, std::ostream& out) const;

  void putTrader(std::string_view mnem, Millis now, std::ostream& out) const;

  void getSess(EntitySet es, Millis now, std::ostream& out) const;

  void getOrder(Millis now, std::ostream& out) const;

  void postOrder(std::string_view market, Millis now, std::ostream& out) const;

  void getTrade(Millis now, std::ostream& out) const;

  void postTrade(std::string_view market, Millis now, std::ostream& out) const;

  void getPosn(Millis now, std::ostream& out) const;

  void getPosn(std::string_view contr, Millis now, std::ostream& out) const;

  void getPosn(std::string_view contr, IsoDate settlDate, Millis now, std::ostream& out) const;

  void getView(Millis now, std::ostream& out) const;

  void getView(std::string_view market, Millis now, std::ostream& out) const;

 private:
  Serv serv_;
};

/** @} */

} // swirly

#endif // SWIRLY_FIR_REST_HPP
