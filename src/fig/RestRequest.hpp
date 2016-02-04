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
#ifndef SWIRLY_FIG_RESTREQUEST_HPP
#define SWIRLY_FIG_RESTREQUEST_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Types.hpp>

namespace swirly {

/**
 * @addtogroup Rest
 * @{
 */

class SWIRLY_API RestRequest {

    int cs_;
    union {
        struct {
            int sign;
            std::size_t digits;
        } num_;
        struct {
            std::size_t* len;
            char* buf;
            std::size_t max;
        } str_;
    };
    unsigned fields_;

    Mnem::Data mnem_;
    Display::Data display_;
    Email::Data email_;
    Mnem::Data trader_;
    Mnem::Data contr_;
    IsoDate settlDate_;
    IsoDate expiryDate_;
    Ref::Data ref_;
    MarketState state_;
    Side side_;
    Lots lots_;
    Ticks ticks_;
    Lots minLots_;
    Role role_;
    Mnem::Data cpty_;

    long num() const noexcept
    {
        return num_.sign * num_.digits;
    }

 public:
    enum : unsigned {
        MNEM = 1 << 0,
        DISPLAY = 1 << 1,
        EMAIL = 1 << 2,
        TRADER = 1 << 3,
        CONTR = 1 << 4,
        SETTL_DATE = 1 << 5,
        EXPIRY_DATE = 1 << 6,
        REF = 1 << 7,
        STATE = 1 << 8,
        SIDE = 1 << 9,
        LOTS = 1 << 10,
        TICKS = 1 << 11,
        MIN_LOTS = 1 << 12,
        ROLE = 1 << 13,
        CPTY = 1 << 14
    };

    RestRequest() noexcept
    {
        reset();
    }
    ~RestRequest() noexcept;

    // Copy.
    RestRequest(const RestRequest&) = delete;
    RestRequest& operator=(const RestRequest&) = delete;

    // Move.
    RestRequest(RestRequest&&) = delete;
    RestRequest& operator=(RestRequest&&) = delete;

    void reset(bool clear = true) noexcept;

    bool parse(const StringView& buf);

    unsigned fields() const noexcept
    {
        return fields_;
    }
    StringView mnem() const noexcept
    {
        return {mnem_.buf, mnem_.len};
    }
    StringView display() const noexcept
    {
        return {display_.buf, display_.len};
    }
    StringView email() const noexcept
    {
        return {email_.buf, email_.len};
    }
    StringView trader() const noexcept
    {
        return {trader_.buf, trader_.len};
    }
    StringView contr() const noexcept
    {
        return {contr_.buf, contr_.len};
    }
    IsoDate settlDate() const noexcept
    {
        return settlDate_;
    }
    IsoDate expiryDate() const noexcept
    {
        return expiryDate_;
    }
    StringView ref() const noexcept
    {
        return {ref_.buf, ref_.len};
    }
    MarketState state() const noexcept
    {
        return state_;
    }
    Side side() const noexcept
    {
        return side_;
    }
    Lots lots() const noexcept
    {
        return lots_;
    }
    Ticks ticks() const noexcept
    {
        return ticks_;
    }
    Lots minLots() const noexcept
    {
        return minLots_;
    }
    Role role() const noexcept
    {
        return role_;
    }
    StringView cpty() const noexcept
    {
        return {cpty_.buf, cpty_.len};
    }
};

/** @} */

} // swirly

#endif // SWIRLY_FIG_RESTREQUEST_HPP
