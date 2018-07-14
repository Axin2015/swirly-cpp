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
#include "Instr.hpp"

#include <swirly/fin/Conv.hpp>

#include <swirly/util/Stream.hpp>

namespace swirly {
inline namespace fin {
using namespace std;

static_assert(sizeof(Instr) <= 4 * 64, "no greater than specified cache-lines");

Instr::Instr(Id32 id, Symbol symbol, string_view display, Symbol base_asset, Symbol term_ccy,
             int lot_numer, int lot_denom, int tick_numer, int tick_denom, int pip_dp,
             Lots min_lots, Lots max_lots) noexcept
: id_{id}
, symbol_{symbol}
, display_{display}
, base_asset_{base_asset}
, term_ccy_{term_ccy}
, lot_numer_{lot_numer}
, lot_denom_{lot_denom}
, qty_inc_{fract_to_real(lot_numer, lot_denom)}
, tick_numer_{tick_numer}
, tick_denom_{tick_denom}
, price_inc_{fract_to_real(tick_numer, tick_denom)}
, pip_dp_{pip_dp}
, qty_dp_{real_to_dp(qty_inc_)}
, price_dp_{real_to_dp(price_inc_)}
, min_lots_{min_lots}
, max_lots_{max_lots}
{
}

Instr::~Instr() = default;

Instr::Instr(const Instr&) = default;

Instr::Instr(Instr&&) = default;

void Instr::to_dsv(ostream& os, char delim) const
{
    OStreamJoiner osj{os, delim};
    osj << id_         //
        << symbol_     //
        << display_    //
        << base_asset_ //
        << term_ccy_   //
        << lot_numer_  //
        << lot_denom_  //
        << tick_numer_ //
        << tick_denom_ //
        << pip_dp_     //
        << min_lots_   //
        << max_lots_;
}

void Instr::to_json(ostream& os) const
{
    os << "{\"symbol\":\"" << symbol_           //
       << "\",\"display\":\"" << display_       //
       << "\",\"base_asset\":\"" << base_asset_ //
       << "\",\"term_ccy\":\"" << term_ccy_     //
       << "\",\"lot_numer\":" << lot_numer_     //
       << ",\"lot_denom\":" << lot_denom_       //
       << ",\"tick_numer\":" << tick_numer_     //
       << ",\"tick_denom\":" << tick_denom_     //
       << ",\"pip_dp\":" << pip_dp_             //
       << ",\"min_lots\":" << min_lots_         //
       << ",\"max_lots\":" << max_lots_         //
       << "}";
}

} // namespace fin
} // namespace swirly
