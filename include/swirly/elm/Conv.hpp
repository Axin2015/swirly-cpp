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
#ifndef SWIRLY_ELM_CONV_HPP
#define SWIRLY_ELM_CONV_HPP

#include <swirly/elm/Types.hpp>

#include <cmath>

namespace swirly {

/**
 * @addtogroup Domain
 * @{
 */

constexpr int64_t roundHalfAway(double real) noexcept
{
  return static_cast<int64_t>(real < 0.0 ? real - 0.5 : real + 0.5);
}

constexpr double fractToReal(int numer, int denom) noexcept
{
  return static_cast<double>(numer) / static_cast<double>(denom);
}

constexpr Incs realToIncs(double real, double incSize) noexcept
{
  return roundHalfAway(real / incSize);
}

constexpr double incsToReal(Incs incs, double incSize) noexcept
{
  return incs * incSize;
}

/**
 * Convert quantity to lots.
 */
constexpr Lots qtyToLots(double qty, double qtyInc) noexcept
{
  return box<Lots>(realToIncs(qty, qtyInc));
}

/**
 * Convert lots to quantity.
 */
constexpr double lotsToQty(Lots lots, double qtyInc) noexcept
{
  return incsToReal(unbox(lots), qtyInc);
}

/**
 * Convert price to ticks.
 */
constexpr Ticks priceToTicks(double price, double priceInc) noexcept
{
  return box<Ticks>(realToIncs(price, priceInc));
}

/**
 * Convert ticks to price.
 */
constexpr double ticksToPrice(Ticks ticks, double priceInc) noexcept
{
  return incsToReal(unbox(ticks), priceInc);
}

/**
 * Number of decimal places in real.
 */
constexpr int realToDp(double d) noexcept
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
inline double dpToReal(int dp) noexcept
{
  return std::pow(10, -dp);
}

/**
 * Cost from lots and ticks.
 */
inline Cost cost(Lots lots, Ticks ticks) noexcept
{
  return box<Cost>(unbox(lots) * unbox(ticks));
}

/** @} */

} // swirly

#endif // SWIRLY_ELM_CONV_HPP
