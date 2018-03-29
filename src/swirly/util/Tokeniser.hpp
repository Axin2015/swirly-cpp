/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#ifndef SWIRLY_UTIL_TOKENISER_HPP
#define SWIRLY_UTIL_TOKENISER_HPP

#include <swirly/util/String.hpp>

#include <algorithm>
#include <array>

namespace swirly {
inline namespace util {

class Tokeniser {
  public:
    Tokeniser(std::string_view buf, std::string_view seps) noexcept { reset(buf, seps); }
    Tokeniser() noexcept { reset(""sv, ""sv); }
    ~Tokeniser() = default;

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
        seps_ = toks;
        i_ = buf_.cbegin();
        j_ = std::find_first_of(i_, buf_.cend(), seps_.cbegin(), seps_.cend());
    }
    void pop() noexcept
    {
        if (j_ != buf_.cend()) {
            i_ = j_ + 1;
            j_ = std::find_first_of(i_, buf_.cend(), seps_.cbegin(), seps_.cend());
        } else {
            i_ = j_;
        }
    }

  private:
    std::string_view buf_;
    std::string_view seps_;
    std::string_view::const_iterator i_, j_;
};

template <std::size_t N>
using Row = std::array<std::string_view, N>;

template <std::size_t N>
void split(std::string_view line, std::string_view seps, Row<N>& row) noexcept
{
    Tokeniser toks{line, seps};
    for (auto& col : row) {
        if (toks.empty()) {
            break;
        }
        col = toks.top();
        toks.pop();
    }
}

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_TOKENISER_HPP
