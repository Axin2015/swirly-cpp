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
#ifndef SWIRLY_WS_RESTREQUEST_HPP
#define SWIRLY_WS_RESTREQUEST_HPP

#include <swirly/fin/Types.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/Mnem.hpp>

namespace swirly {

class SWIRLY_API RestRequest {
 public:
  enum : unsigned {
    Mnem = 1 << 0,
    Contr = 1 << 1,
    SettlDate = 1 << 2,
    Accnt = 1 << 3,
    Ref = 1 << 4,
    State = 1 << 5,
    Side = 1 << 6,
    Lots = 1 << 7,
    Ticks = 1 << 8,
    MinLots = 1 << 9,
    LiqInd = 1 << 10,
    Cpty = 1 << 11
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
  swirly::Mnem mnem() const noexcept { return +mnem_; }
  swirly::Mnem contr() const noexcept { return +contr_; }
  IsoDate settlDate() const noexcept { return settlDate_; }
  swirly::Mnem accnt() const noexcept { return +accnt_; }
  std::string_view ref() const noexcept { return +ref_; }
  MarketState state() const noexcept { return state_; }
  swirly::Side side() const noexcept { return side_; }
  swirly::Lots lots() const noexcept { return lots_; }
  swirly::Ticks ticks() const noexcept { return ticks_; }
  swirly::Lots minLots() const noexcept { return minLots_; }
  swirly::LiqInd liqInd() const noexcept { return liqInd_; }
  swirly::Mnem cpty() const noexcept { return +cpty_; }
  /**
   * Validate fields.
   *
   * @param required Required fields.
   *
   * @param optional Optional fields.
   *
   * @return true if fields are value.
   */
  bool valid(unsigned required, unsigned optional = 0x0) const noexcept
  {
    return (fields_ & required) == required && (fields_ & ~(required | optional)) == 0;
  }
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
      int* len;
      char* buf;
      int max;
    } str_;
  };
  unsigned fields_;

  StringData<MaxMnem> mnem_;
  StringData<MaxMnem> contr_;
  IsoDate settlDate_;
  StringData<MaxMnem> accnt_;
  StringData<MaxRef> ref_;
  MarketState state_;
  swirly::Side side_;
  swirly::Lots lots_;
  swirly::Ticks ticks_;
  swirly::Lots minLots_;
  swirly::LiqInd liqInd_;
  StringData<MaxMnem> cpty_;

  long num() const noexcept { return num_.sign * num_.digits; }
};

} // swirly

#endif // SWIRLY_WS_RESTREQUEST_HPP
