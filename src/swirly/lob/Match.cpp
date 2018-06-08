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
#include "Match.hxx"

#include <swirly/fin/Exec.hpp>
#include <swirly/fin/Order.hpp>
#include <swirly/fin/Posn.hpp>

namespace swirly {
inline namespace lob {

Match::Match(Lots lots, const OrderPtr& maker_order, const ExecPtr& maker_trade,
             const PosnPtr& maker_posn, const ExecPtr& taker_trade) noexcept
: lots{lots}
, maker_order{maker_order}
, maker_trade{maker_trade}
, maker_posn{maker_posn}
, taker_trade{taker_trade}
{
}

Match::~Match() = default;

Match::Match(const Match&) = default;

Match::Match(Match&&) = default;

} // namespace lob
} // namespace swirly
