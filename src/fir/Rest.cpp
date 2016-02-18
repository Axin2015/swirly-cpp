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
#include <swirly/fir/Rest.hpp>

#include <swirly/ash/Stream.hpp>

using namespace std;

namespace swirly {

Rest::~Rest() noexcept = default;

Rest::Rest(Rest&&) = default;

Rest& Rest::operator=(Rest&&) = default;

void Rest::assets(Millis now, std::ostream& out) const
{
  const auto& assets = serv_.assets();
  out << '[';
  copy(assets.begin(), assets.end(), OStreamJoiner(out, ','));
  out << ']';
}

void Rest::contrs(Millis now, std::ostream& out) const
{
  const auto& contrs = serv_.contrs();
  out << '[';
  copy(contrs.begin(), contrs.end(), OStreamJoiner(out, ','));
  out << ']';
}

} // swirly
