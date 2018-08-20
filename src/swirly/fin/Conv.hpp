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
#ifndef SWIRLY_FIN_CONV_HPP
#define SWIRLY_FIN_CONV_HPP

#include <swirly/fin/IntTypes.hpp>

#include <cmath>

namespace swirly {
inline namespace fin {

constexpr int64_t round_half_away(double real) noexcept
{
    return static_cast<int64_t>(real < 0.0 ? real - 0.5 : real + 0.5);
}

constexpr double fract_to_real(int numer, int denom) noexcept
{
    return static_cast<double>(numer) / static_cast<double>(denom);
}

constexpr double fract_to_real(Incs numer, Incs denom) noexcept
{
    return static_cast<double>(numer) / static_cast<double>(denom);
}

constexpr Incs real_to_incs(double real, double inc_size) noexcept
{
    return round_half_away(real / inc_size);
}

constexpr double incs_to_real(Incs incs, double inc_size) noexcept
{
    return incs * inc_size;
}

/**
 * Convert quantity to lots.
 */
constexpr Lots qty_to_lots(double qty, double qty_inc) noexcept
{
    return Lots{real_to_incs(qty, qty_inc)};
}

/**
 * Convert lots to quantity.
 */
constexpr double lots_to_qty(Lots lots, double qty_inc) noexcept
{
    return incs_to_real(lots.count(), qty_inc);
}

/**
 * Convert price to ticks.
 */
constexpr Ticks price_to_ticks(double price, double price_inc) noexcept
{
    return Ticks{real_to_incs(price, price_inc)};
}

/**
 * Convert ticks to price.
 */
constexpr double ticks_to_price(Ticks ticks, double price_inc) noexcept
{
    return incs_to_real(ticks.count(), price_inc);
}

/**
 * Number of decimal places in real.
 */
inline int real_to_dp(double d) noexcept
{
    int dp{0};
    for (; dp < 9; ++dp) {
        double ip{};
        const double fp{std::modf(d, &ip)};
        if (fp < 0.000000001) {
            break;
        }
        d *= 10;
    }
    return dp;
}

/**
 * Decimal places as real.
 */
inline double dp_to_real(int dp) noexcept
{
    return std::pow(10, -dp);
}

/**
 * Cost from lots and ticks.
 */
constexpr Cost cost(Lots lots, Ticks ticks) noexcept
{
    return Cost{lots.count() * ticks.count()};
}

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_CONV_HPP
