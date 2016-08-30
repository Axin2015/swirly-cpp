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
#ifndef SWIRLY_ELM_MARKET_HPP
#define SWIRLY_ELM_MARKET_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Mnem.hpp>
#include <swirly/ash/Types.hpp>

namespace swirly {

/**
 * A place where buyers and sellers come together to exchange goods or services.
 */
class SWIRLY_API Market : public Comparable<Market> {
 public:
  Market(Mnem mnem, std::string_view display, Mnem contr, JDay settlDay, JDay expiryDay,
         MarketState state) noexcept;

  virtual ~Market() noexcept;

  // Copy.
  Market(const Market&);
  Market& operator=(const Market&) = delete;

  // Move.
  Market(Market&&);
  Market& operator=(Market&&) = delete;

  template <typename... ArgsT>
  static MarketPtr make(ArgsT&&... args)
  {
    return std::make_unique<Market>(std::forward<ArgsT>(args)...);
  }

  void toJson(std::ostream& os) const;

  int compare(const Market& rhs) const noexcept { return mnem_.compare(rhs.mnem_); }
  auto mnem() const noexcept { return mnem_; }
  auto display() const noexcept { return +display_; }
  auto contr() const noexcept { return contr_; }
  auto settlDay() const noexcept { return settlDay_; }
  auto expiryDay() const noexcept { return expiryDay_; }
  auto state() const noexcept { return state_; }
  void setDisplay(std::string_view display) noexcept { display_ = display; }
  void setState(MarketState state) noexcept { state_ = state; }

 protected:
  const Mnem mnem_;
  Display display_;
  const Mnem contr_;
  const JDay settlDay_;
  const JDay expiryDay_;
  MarketState state_;
};

inline std::ostream& operator<<(std::ostream& os, const Market& market)
{
  market.toJson(os);
  return os;
}

} // swirly

#endif // SWIRLY_ELM_MARKET_HPP
