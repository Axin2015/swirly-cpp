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
#ifndef SWIRLY_LOB_MATCH_HXX
#define SWIRLY_LOB_MATCH_HXX

#include <swirly/fin/Types.hpp>

namespace swirly {
inline namespace lob {

struct Match {
    Match(Lots lots, const OrderPtr& maker_order, const ExecPtr& maker_trade,
          const PosnPtr& maker_posn, const ExecPtr& taker_trade) noexcept;

    ~Match();

    // Copy.
    Match(const Match&);
    Match& operator=(const Match&) = delete;

    // Move.
    Match(Match&&);
    Match& operator=(Match&&) = delete;

    const Lots lots;
    const OrderPtr maker_order;
    const ExecPtr maker_trade;
    const PosnPtr maker_posn;
    const ExecPtr taker_trade;
};

} // namespace lob
} // namespace swirly

#endif // SWIRLY_LOB_MATCH_HXX
