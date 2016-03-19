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
#ifndef SWIRLY_FIR_RESTREQUEST_HPP
#define SWIRLY_FIR_RESTREQUEST_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Types.hpp>

/**
 * @addtogroup Rest
 * @{
 */

namespace swirly {

class SWIRLY_API RestRequest {
 public:
  enum : unsigned {
    Mnem = 1 << 0,
    Display = 1 << 1,
    Email = 1 << 2,
    Trader = 1 << 3,
    Contr = 1 << 4,
    SettlDate = 1 << 5,
    ExpiryDate = 1 << 6,
    Ref = 1 << 7,
    State = 1 << 8,
    Side = 1 << 9,
    Lots = 1 << 10,
    Ticks = 1 << 11,
    MinLots = 1 << 12,
    Role = 1 << 13,
    Cpty = 1 << 14
  };

  RestRequest() noexcept { reset(); }
  ~RestRequest() noexcept;

  // Copy.
  RestRequest(const RestRequest&) = delete;
  RestRequest& operator=(const RestRequest&) = delete;

  // Move.
  RestRequest(RestRequest&&) = delete;
  RestRequest& operator=(RestRequest&&) = delete;

  unsigned fields() const noexcept { return fields_; }
  std::string_view mnem() const noexcept { return +mnem_; }
  std::string_view display() const noexcept { return +display_; }
  std::string_view email() const noexcept { return +email_; }
  std::string_view trader() const noexcept { return +trader_; }
  std::string_view contr() const noexcept { return +contr_; }
  IsoDate settlDate() const noexcept { return settlDate_; }
  IsoDate expiryDate() const noexcept { return expiryDate_; }
  std::string_view ref() const noexcept { return +ref_; }
  MarketState state() const noexcept { return state_; }
  swirly::Side side() const noexcept { return side_; }
  swirly::Lots lots() const noexcept { return lots_; }
  swirly::Ticks ticks() const noexcept { return ticks_; }
  swirly::Lots minLots() const noexcept { return minLots_; }
  swirly::Role role() const noexcept { return role_; }
  std::string_view cpty() const noexcept { return +cpty_; }
  void reset(bool clear = true) noexcept;

  bool parse(std::string_view buf);

 private:
  int cs_;
  union {
    struct {
      int sign;
      std::size_t digits;
    } num_;
    struct {
      std::size_t* len;
      char* buf;
      std::size_t max;
    } str_;
  };
  unsigned fields_;

  StringData<MnemMax> mnem_;
  StringData<DisplayMax> display_;
  StringData<EmailMax> email_;
  StringData<MnemMax> trader_;
  StringData<MnemMax> contr_;
  IsoDate settlDate_;
  IsoDate expiryDate_;
  StringData<RefMax> ref_;
  MarketState state_;
  swirly::Side side_;
  swirly::Lots lots_;
  swirly::Ticks ticks_;
  swirly::Lots minLots_;
  swirly::Role role_;
  StringData<MnemMax> cpty_;

  long num() const noexcept { return num_.sign * num_.digits; }
};

} // swirly

/** @} */

#endif // SWIRLY_FIR_RESTREQUEST_HPP
