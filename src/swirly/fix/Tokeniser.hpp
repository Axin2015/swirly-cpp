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
#ifndef SWIRLY_FIX_TOKENISER_HPP
#define SWIRLY_FIX_TOKENISER_HPP

#include <swirly/fix/Exception.hpp>
#include <swirly/fix/Types.hpp>

#include <cassert>
#include <cctype>

namespace swirly {
inline namespace fix {

class FixTokeniser {
  public:
    explicit FixTokeniser(std::string_view msg)
    : msg_{msg}
    , it_{msg_.cbegin()}
    {
    }
    // Copy.
    FixTokeniser(const FixTokeniser&) = delete;
    FixTokeniser& operator=(const FixTokeniser&) = delete;

    // Move.
    FixTokeniser(FixTokeniser&&) = delete;
    FixTokeniser& operator=(FixTokeniser&&) = delete;

    bool empty() const noexcept { return it_ == msg_.cend(); }
    FixPair next() { return {tag(), value()}; }

  private:
    int tag()
    {
        int tag{0};
        assert(it_ != msg_.cend());
        if (std::isdigit(*it_)) {
            tag += *it_++ - '0';
            for (;;) {
                if (it_ == msg_.cend()) {
                    throw FixException{"partial FIX tag"};
                }
                if (!std::isdigit(*it_)) {
                    break;
                }
                tag *= 10;
                tag += *it_++ - '0';
            }
        }
        // Verify that first non-digit charactor was '=' delimiter.
        if (*it_ != '=') {
            throw FixException{"invalid FIX tag"};
        }
        // Skip delimiter.
        ++it_;
        return tag;
    }
    std::string_view value()
    {
        const auto begin = it_;
        for (; it_ != msg_.cend(); ++it_) {
            if (*it_ == '\1') {
                return msg_.substr(begin - msg_.cbegin(), it_++ - begin);
            }
        }
        throw FixException{"partial FIX value"};
    }
    std::string_view msg_{};
    std::string_view::const_iterator it_{};
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_TOKENISER_HPP
