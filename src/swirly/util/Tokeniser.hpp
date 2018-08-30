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
#include <swirly/util/Time.hpp>

#include <algorithm>
#include <array>

namespace swirly {
inline namespace util {

class Tokeniser {
  public:
    Tokeniser(std::string_view buf, std::string_view delims) noexcept { reset(buf, delims); }
    Tokeniser() noexcept { reset(""sv, ""sv); }
    ~Tokeniser() = default;

    // Copy.
    Tokeniser(const Tokeniser& rhs) noexcept = default;
    Tokeniser& operator=(const Tokeniser& rhs) noexcept = default;

    // Move.
    Tokeniser(Tokeniser&&) noexcept = default;
    Tokeniser& operator=(Tokeniser&&) noexcept = default;

    void reset(std::string_view buf, std::string_view delims) noexcept
    {
        buf_ = buf;
        delims_ = delims;
        i_ = buf_.cbegin();
        j_ = std::find_first_of(i_, buf_.cend(), delims_.cbegin(), delims_.cend());
    }
    /**
     * Returns total bytes consumed.
     */
    std::size_t consumed() const noexcept { return i_ - buf_.cbegin(); }
    /**
     * Returns true if all bytes have been consumed.
     */
    bool empty() const noexcept { return i_ == buf_.cend(); }
    /**
     * Returns true if a delimiter was found in the remaining data.
     */
    bool has_delim() const noexcept { return j_ != buf_.cend(); }
    std::string_view top() const noexcept { return buf_.substr(i_ - buf_.cbegin(), j_ - i_); }
    std::string_view next() noexcept
    {
        const auto tok = top();
        pop();
        return tok;
    }
    void pop() noexcept
    {
        if (j_ != buf_.cend()) {
            i_ = j_ + 1;
            j_ = std::find_first_of(i_, buf_.cend(), delims_.cbegin(), delims_.cend());
        } else {
            i_ = j_;
        }
    }

  private:
    std::string_view buf_;
    std::string_view delims_;
    // Assumption: std::string_view iterators are equivalent across copies of the std::string_view.
    std::string_view::const_iterator i_, j_;
};

template <typename FnT>
std::size_t parseLine(std::string_view buf, Time now, FnT fn)
{
    Tokeniser lines{buf, "\n"sv};
    while (lines.has_delim()) {
        fn(lines.top(), now);
        lines.pop();
    }
    return lines.consumed();
}

template <std::size_t N>
using Row = std::array<std::string_view, N>;

template <std::size_t N>
void split(std::string_view line, std::string_view delims, Row<N>& row) noexcept
{
    Tokeniser toks{line, delims};
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
