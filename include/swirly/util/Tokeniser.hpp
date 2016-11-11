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
#ifndef SWIRLY_ASH_TOKENISER_HPP
#define SWIRLY_ASH_TOKENISER_HPP

#include <swirly/util/String.hpp>

#include <algorithm>

namespace swirly {

class Tokeniser {
 public:
  Tokeniser(std::string_view buf, std::string_view toks) noexcept { reset(buf, toks); }
  Tokeniser() noexcept { reset(""_sv, ""_sv); }
  ~Tokeniser() noexcept = default;

  // Copy.
  Tokeniser(const Tokeniser& rhs) noexcept = default;
  Tokeniser& operator=(const Tokeniser& rhs) noexcept = default;

  // Move.
  Tokeniser(Tokeniser&&) noexcept = default;
  Tokeniser& operator=(Tokeniser&&) noexcept = default;

  std::string_view top() const noexcept { return buf_.substr(i_ - buf_.cbegin(), j_ - i_); }
  bool empty() const noexcept { return i_ == buf_.cend(); }
  void reset(std::string_view buf, std::string_view toks) noexcept
  {
    buf_ = buf;
    toks_ = toks;
    i_ = buf_.cbegin();
    j_ = std::find_first_of(i_, buf_.cend(), toks_.cbegin(), toks_.cend());
  }
  void pop() noexcept
  {
    if (j_ != buf_.cend()) {
      i_ = j_ + 1;
      j_ = std::find_first_of(i_, buf_.cend(), toks_.cbegin(), toks_.cend());
    } else {
      i_ = j_;
    }
  }

 private:
  std::string_view buf_;
  std::string_view toks_;
  std::string_view::const_iterator i_, j_;
};

} // swirly

#endif // SWIRLY_ASH_TOKENISER_HPP
