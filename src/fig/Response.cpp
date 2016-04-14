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
#include <swirly/fig/Response.hpp>

#include <swirly/elm/Exec.hpp>
#include <swirly/elm/MarketBook.hpp>
#include <swirly/elm/Order.hpp>
#include <swirly/elm/Posn.hpp>

#include <swirly/ash/Stream.hpp>

#include <algorithm> // transform()

using namespace std;

namespace swirly {

Response::Response() noexcept = default;

Response::~Response() noexcept = default;

Response::Response(const Response&) = default;

Response& Response::operator=(const Response&) = default;

Response::Response(Response&&) noexcept = default;

Response& Response::operator=(Response&&) noexcept = default;

void Response::toJson(ostream& os) const
{
  os << "{\"orders\":[";
  transform(orders_.begin(), orders_.end(), OStreamJoiner(os, ','),
            [](const auto& ptr) -> const auto& { return *ptr; });
  os << "],\"execs\":[";
  transform(execs_.begin(), execs_.end(), OStreamJoiner(os, ','),
            [](const auto& ptr) -> const auto& { return *ptr; });
  os << "],\"posn\":";
  if (posn_) {
    os << *posn_;
  } else {
    os << "null";
  }
  os << ",\"view\":";
  assert(book_);
  book_->toJsonView(os);
  os << '}';
}

ConstPosnPtr Response::posn() const noexcept
{
  return posn_;
}

void Response::setPosn(PosnPtr posn) noexcept
{
  posn_ = posn;
}

void Response::clear() noexcept
{
  book_ = nullptr;
  orders_.clear();
  execs_.clear();
  posn_ = nullptr;
}

void Response::clearMatches() noexcept
{
  if (!orders_.empty()) {
    orders_.resize(1);
  }
  if (!execs_.empty()) {
    execs_.resize(1);
  }
  posn_ = nullptr;
}

void Response::insertOrder(ConstOrderPtr order)
{
  orders_.push_back(order);
}

void Response::insertExec(ConstExecPtr exec)
{
  execs_.push_back(exec);
}

} // swirly
