/*
 * Swirly Order-Book and Matching-Engine.
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

#include <swirly/ash/String.hpp>

#include <algorithm>

namespace swirly {

/**
 * @addtogroup Util
 * @{
 */

template <char DelimN>
class Tokeniser {
    StringView buf_;
    StringView::const_iterator i_, j_;

 public:
    explicit Tokeniser(const StringView& buf) noexcept
    {
        reset(buf);
    }
    explicit Tokeniser() noexcept
    {
        reset("");
    }
    ~Tokeniser() noexcept = default;

    // Copy.
    Tokeniser(const Tokeniser& rhs) noexcept = default;
    Tokeniser& operator=(const Tokeniser& rhs) noexcept = default;

    // Move.
    Tokeniser(Tokeniser&&) noexcept = default;
    Tokeniser& operator=(Tokeniser&&) noexcept = default;

    void reset(const StringView& buf) noexcept
    {
        buf_ = buf;
        i_ = buf_.cbegin();
        j_ = std::find(i_, buf_.cend(), DelimN);
    }
    void pop() noexcept
    {
        if (j_ != buf_.cend()) {
            i_ = j_ + 1;
            j_ = std::find(i_, buf_.cend(), DelimN);
        } else
            i_ = j_;
    }
    StringView top() const noexcept
    {
        return buf_.substr(i_ - buf_.cbegin(), j_ - i_);
    }
    bool empty() const noexcept
    {
        return i_ == buf_.cend();
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ASH_TOKENISER_HPP
