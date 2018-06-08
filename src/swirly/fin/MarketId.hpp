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
#ifndef SWIRLY_FIN_MARKETID_HPP
#define SWIRLY_FIN_MARKETID_HPP

#include <swirly/util/BasicTypes.hpp>
#include <swirly/util/Date.hpp>

namespace swirly {
inline namespace fin {

constexpr Id64 to_market_id(Id32 instr_id, JDay settl_day) noexcept
{
    return Id64{(instr_id.count() << 16) | (jd_to_tjd(settl_day) & 0xffff)};
}

constexpr Id64 to_market_id(Id32 instr_id, IsoDate settl_date) noexcept
{
    return to_market_id(instr_id, maybe_iso_to_jd(settl_date));
}

template <typename ValueT>
struct MarketIdTraits {
    using Id = Id64;
    static Id id(const ValueT& value) noexcept { return value.market_id(); }
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_MARKETID_HPP
