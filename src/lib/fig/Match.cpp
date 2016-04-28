/*
 * The Restful Matching-Engine.
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
#include "Match.hpp"

#include <swirly/elm/Exec.hpp>
#include <swirly/elm/Order.hpp>
#include <swirly/elm/Posn.hpp>

namespace swirly {

Match::Match(Lots lots, const OrderPtr& makerOrder, const ExecPtr& makerTrade,
             const PosnPtr& makerPosn, const ExecPtr& takerTrade) noexcept
  : lots{lots},
    makerOrder{makerOrder},
    makerTrade{makerTrade},
    makerPosn{makerPosn},
    takerTrade{takerTrade}
{
}

Match::~Match() noexcept = default;

Match::Match(const Match&) = default;

Match::Match(Match&&) = default;

} // swirly
