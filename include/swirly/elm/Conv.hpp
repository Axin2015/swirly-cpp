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
#ifndef SWIRLY_ELM_CONV_HPP
#define SWIRLY_ELM_CONV_HPP

#include <swirly/elm/Types.hpp>

#include <cmath>

namespace swirly {

inline constexpr long roundHalfAway(double real)
{
    return static_cast<long>(real < 0.0 ? real - 0.5 : real + 0.5);
}

inline constexpr double fractToReal(int numer, int denom)
{
    return static_cast<double>(numer) / static_cast<double>(denom);
}

inline constexpr Incs realToIncs(double real, double incSize)
{
    return roundHalfAway(real / incSize);
}

inline constexpr double incsToReal(Incs incs, double incSize)
{
    return incs * incSize;
}

/**
 * Convert quantity to lots.
 */
inline constexpr Lots qtyToLots(double qty, double qtyInc)
{
    return realToIncs(qty, qtyInc);
}

/**
 * Convert lots to quantity.
 */
inline constexpr double lotsToQty(Lots lots, double qtyInc)
{
    return incsToReal(lots, qtyInc);
}

/**
 * Convert price to ticks.
 */
inline constexpr Ticks priceToTicks(double price, double priceInc)
{
    return realToIncs(price, priceInc);
}

/**
 * Convert ticks to price.
 */
inline constexpr double ticksToPrice(Ticks ticks, double priceInc)
{
    return incsToReal(ticks, priceInc);
}

/**
 * Number of decimal places in real.
 */
inline constexpr int realToDp(double d)
{
    int dp{0};
    for (; dp < 9; ++dp) {
        double ip{};
        const double fp{std::modf(d, &ip)};
        if (fp < 0.000000001)
            break;
        d *= 10;
    }
    return dp;
}

/**
 * Decimal places as real.
 */
inline double dpToReal(int dp)
{
    return std::pow(10, -dp);
}

} // swirly

#endif // SWIRLY_ELM_CONV_HPP
