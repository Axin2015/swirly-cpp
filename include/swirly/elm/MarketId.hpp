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
#ifndef SWIRLY_ELM_MARKETID_HPP
#define SWIRLY_ELM_MARKETID_HPP

#include <swirly/ash/Compare.hpp>
#include <swirly/ash/Date.hpp>

#include <iostream>

namespace swirly {

class MarketId : public Comparable<MarketId> {
 public:
  constexpr explicit MarketId(int64_t id) noexcept : id_{id} {}
  constexpr MarketId(Id32 contr, JDay settlDay) noexcept
    : id_{(unbox(contr) << 16) | (jdToTjd(settlDay) & 0xffff)}
  {
  }
  ~MarketId() noexcept = default;

  // Copy.
  constexpr MarketId(const MarketId&) noexcept = default;
  MarketId& operator=(const MarketId&) noexcept = default;

  // Move.
  constexpr MarketId(MarketId&&) noexcept = default;
  MarketId& operator=(MarketId&&) noexcept = default;

  constexpr int compare(const MarketId& rhs) const noexcept
  {
    return swirly::compare(id_, rhs.id_);
  }
  constexpr auto id() const noexcept { return id_; }
  constexpr auto contr() const noexcept { return box<Id32>((id_ >> 16) & 0xffffffff); }
  constexpr auto settlDay() const noexcept { return tjdToJd(id_ & 0xffff); }
 private:
  int64_t id_;
};

inline std::ostream& operator<<(std::ostream& os, MarketId market)
{
  os << market.id();
  return os;
}

} // swirly

#endif // SWIRLY_ELM_MARKETID_HPP
