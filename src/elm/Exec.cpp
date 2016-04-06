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
#include <swirly/elm/Exec.hpp>

#include <swirly/ash/JulianDay.hpp>

using namespace std;

namespace swirly {

Exec::~Exec() noexcept = default;

Exec::Exec(Exec&&) = default;

void Exec::toJson(ostream& os) const
{
  os << "{\"trader\":\"" << trader_ //
     << "\",\"market\":\"" << market_ //
     << "\",\"contr\":\"" << contr_ //
     << "\",\"settlDate\":";
  if (settlDay_ != 0_jd) {
    os << jdToIso(settlDay_);
  } else {
    os << "null";
  }
  os << ",\"id\":" << id_ //
     << ",\"ref\":";
  if (!ref_.empty()) {
    os << '"' << ref_ << '"';
  } else {
    os << "null";
  }
  os << ",\"orderId\":" << orderId_ //
     << ",\"state\":\"" << state_ //
     << "\",\"side\":\"" << side_ //
     << "\",\"lots\":" << lots_ //
     << ",\"ticks\":" << ticks_ //
     << ",\"resd\":" << resd_ //
     << ",\"exec\":" << exec_ //
     << ",\"cost\":" << cost_;
  if (lastLots_ != 0_lts) {
    os << ",\"lastLots\":" << lastLots_ //
       << ",\"lastTicks\":" << lastTicks_;
  } else {
    os << ",\"lastLots\":null,\"lastTicks\":null";
  }
  os << ",\"minLots\":";
  if (minLots_ != 0_lts) {
    os << minLots_;
  } else {
    os << "null";
  }
  os << ",\"matchId\":";
  if (matchId_ != 0_id) {
    os << matchId_;
  } else {
    os << "null";
  }
  os << ",\"role\":";
  if (role_ != Role::None) {
    os << '"' << role_ << '"';
  } else {
    os << "null";
  }
  os << ",\"cpty\":";
  if (!cpty_.empty()) {
    os << '"' << cpty_ << '"';
  } else {
    os << "null";
  }
  os << ",\"created\":" << created_ //
     << '}';
}

void Exec::trade(Lots sumLots, Cost sumCost, Lots lastLots, Ticks lastTicks, Iden matchId,
                 Role role, string_view cpty) noexcept
{
  using namespace enumops;
  state_ = State::Trade;
  resd_ -= sumLots;
  exec_ += sumLots;
  cost_ += sumCost;
  lastLots_ = lastLots;
  lastTicks_ = lastTicks;
  matchId_ = matchId;
  role_ = role;
  cpty_ = cpty;
}

} // swirly
