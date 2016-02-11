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
#ifndef SWIRLY_FIG_RESPONSE_HPP
#define SWIRLY_FIG_RESPONSE_HPP

#include <swirly/elm/Types.hpp>

#include <vector>

namespace swirly {

class MarketBook;

/**
 * @addtogroup App
 * @{
 */

class SWIRLY_API Response {
 public:
  Response() noexcept;
  ~Response() noexcept;

  // Copy.
  Response(const Response&);
  Response& operator=(const Response&);

  // Move.
  Response(Response&&) noexcept;
  Response& operator=(Response&&) noexcept;

  const MarketBook& book() const noexcept { return *book_; }
  void reset(const MarketBook& book) noexcept
  {
    book_ = &book;
    clearAll();
  }
  void reset(const MarketBook& book, const OrderPtr& order, const ExecPtr& exec);

  void clearAll() noexcept;

  void clearMatches() noexcept;

 private:
  using Orders = std::vector<OrderPtr>;
  using Execs = std::vector<ExecPtr>;

  const MarketBook* book_{nullptr};
  Orders orders_;
  Execs execs_;
  PosnPtr posn_;
};

/** @} */

} // swirly

#endif // SWIRLY_FIG_RESPONSE_HPP
