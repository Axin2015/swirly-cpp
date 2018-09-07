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
#ifndef SWIRLY_FIX_LEXER_HPP
#define SWIRLY_FIX_LEXER_HPP

#include <swirly/fix/Types.hpp>

#include <swirly/Config.h>

namespace swirly {
inline namespace fix {

SWIRLY_API std::pair<std::string_view, bool> find_tag(std::string_view msg, int tag) noexcept;

class SWIRLY_API FixLexer {
    using ConstIterator = std::string_view::const_iterator;

  public:
    explicit FixLexer(std::string_view msg, std::size_t offset = 0)
    : msg_{msg}
    , i_{msg_.cbegin() + offset}
    , j_{i_}
    {
        assert(offset == 0 || offset < msg.size());
    }
    ~FixLexer() = default;

    // Copy.
    FixLexer(const FixLexer&) = default;
    FixLexer& operator=(const FixLexer&) = default;

    // Move.
    FixLexer(FixLexer&&) = default;
    FixLexer& operator=(FixLexer&&) = default;

    bool empty() const noexcept { return i_ == msg_.cend(); }
    std::size_t offset() const noexcept { return i_ - msg_.cbegin(); }
    std::pair<std::string_view, bool> find(int tag) const noexcept { return find_tag(msg_, tag); }
    FixPair top() const
    {
        FixPair tv;
        j_ = get_value(get_tag(i_, tv.first), tv.second);
        return tv;
    }
    FixPair next()
    {
        FixPair tv;
        i_ = j_ = get_value(get_tag(i_, tv.first), tv.second);
        return tv;
    }
    void pop() noexcept;

  private:
    ConstIterator get_tag(ConstIterator it, int& val) const;
    ConstIterator get_value(ConstIterator it, std::string_view& val) const noexcept;
    std::string_view msg_{};
    // Assumption: std::string_view iterators are equivalent across copies of the std::string_view.
    ConstIterator i_{};
    mutable ConstIterator j_{};
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_LEXER_HPP
