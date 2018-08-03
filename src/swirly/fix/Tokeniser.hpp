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

#include <swirly/fix/Types.hpp>

#include <swirly/Config.h>

namespace swirly {
inline namespace fix {

SWIRLY_API std::pair<std::string_view, bool> find_tag(std::string_view msg, int tag) noexcept;

class SWIRLY_API FixTokeniser {
  public:
    explicit FixTokeniser(std::string_view msg)
    : msg_{msg}
    , it_{msg_.cbegin()}
    {
    }
    // Copy.
    FixTokeniser(const FixTokeniser&) = default;
    FixTokeniser& operator=(const FixTokeniser&) = default;

    // Move.
    FixTokeniser(FixTokeniser&&) = default;
    FixTokeniser& operator=(FixTokeniser&&) = default;

    bool empty() const noexcept { return it_ == msg_.cend(); }
    std::pair<std::string_view, bool> find(int tag) const noexcept { return find_tag(msg_, tag); }
    FixPair next() { return {tag(), value()}; }

  private:
    int tag(std::string_view::const_iterator& it) const;
    std::string_view value(std::string_view::const_iterator& it) const noexcept;
    int tag() { return tag(it_); }
    std::string_view value() noexcept { return value(it_); }
    std::string_view msg_{};
    std::string_view::const_iterator it_{};
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_TOKENISER_HPP
