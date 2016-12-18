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
#ifndef SWIRLY_CLOB_RESPONSE_HPP
#define SWIRLY_CLOB_RESPONSE_HPP

#include <swirly/fin/Types.hpp>

#include <vector>

namespace swirly {

class SWIRLY_API Response {
 public:
  using Orders = std::vector<ConstOrderPtr>;
  using Execs = std::vector<ConstExecPtr>;

  Response() noexcept;
  ~Response() noexcept;

  // Copy.
  Response(const Response&);
  Response& operator=(const Response&);

  // Move.
  Response(Response&&) noexcept;
  Response& operator=(Response&&) noexcept;

  void toJson(std::ostream& os) const;

  ConstMarketPtr market() const noexcept;
  const Orders& orders() const noexcept { return orders_; }
  const Execs& execs() const noexcept { return execs_; }
  ConstPosnPtr posn() const noexcept;

  void clear() noexcept;

  void clearMatches() noexcept;

  void setMarket(const ConstMarketPtr& market) noexcept;

  void insertOrder(const ConstOrderPtr& order);

  void insertExec(const ConstExecPtr& exec);

  void setPosn(const ConstPosnPtr& posn) noexcept;

 private:
  ConstMarketPtr market_;
  Orders orders_;
  Execs execs_;
  ConstPosnPtr posn_;
};

inline std::ostream& operator<<(std::ostream& os, const Response& resp)
{
  resp.toJson(os);
  return os;
}

} // swirly

#endif // SWIRLY_CLOB_RESPONSE_HPP
