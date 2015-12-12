/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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
#ifndef SWIRLY_ELM_MARKET_HPP
#define SWIRLY_ELM_MARKET_HPP

#include <swirly/elm/rec.hpp>

#include <swirly/ash/types.hpp>

namespace swirly {

class SWIRLY_API Market : public Rec {
protected:
    const Mnem contr_;
    const Jd settlDay_;
    const Jd expiryDay_;
    int state_;

public:
    Market(const char* mnem, const char* display, const char* contr, Jd settlDay, Jd expiryDay,
           int state) noexcept;

    ~Market() noexcept override;

    // Copy.
    Market(const Market&) = default;
    Market& operator =(const Market&) = default;

    // Move.
    Market(Market&&) = default;
    Market& operator =(Market&&) = default;

    StringView contr() const noexcept
    {
        return contr_;
    }
    Jd settlDay() const noexcept
    {
        return settlDay_;
    }
    Jd expiryDay() const noexcept
    {
        return expiryDay_;
    }
    int state() const noexcept
    {
        return state_;
    }
};

} // swirly

#endif // SWIRLY_ELM_MARKET_HPP
