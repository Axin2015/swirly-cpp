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
#ifndef SWIRLY_ELM_MSG_HPP
#define SWIRLY_ELM_MSG_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/BasicTypes.hpp>
#include <swirly/ash/Date.hpp>
#include <swirly/ash/Mnem.hpp>
#include <swirly/ash/Pipe.hpp>

namespace swirly {

enum class MsgType : int { Reset, CreateMarket, UpdateMarket, CreateExec, ArchiveTrade };

struct SWIRLY_PACKED CreateMarketBody {
  Id64 id;
  char contr[MaxMnem];
  JDay settlDay;
  MarketState state;
};
static_assert(std::is_pod<CreateMarketBody>::value, "message-type must be pod");

struct SWIRLY_PACKED UpdateMarketBody {
  Id64 id;
  MarketState state;
};
static_assert(std::is_pod<UpdateMarketBody>::value, "message-type must be pod");

struct SWIRLY_PACKED CreateExecBody {
  Id64 marketId;
  char contr[MaxMnem];
  JDay settlDay;
  Id64 id;
  Id64 orderId;
  char accnt[MaxMnem];
  char ref[MaxRef];
  State state;
  Side side;
  Lots lots;
  Ticks ticks;
  Lots resd;
  Lots exec;
  Cost cost;
  Lots lastLots;
  Ticks lastTicks;
  Lots minLots;
  Id64 matchId;
  LiqInd liqInd;
  char cpty[MaxMnem];
  // std::chrono::time_point is not pod.
  int64_t created;
  More more;
};
static_assert(std::is_pod<CreateExecBody>::value, "message-type must be pod");

constexpr std::size_t MaxIds{(sizeof(CreateExecBody) - MaxMnem - sizeof(int64_t) - sizeof(More))
                             / sizeof(Id64)};
struct SWIRLY_PACKED ArchiveTradeBody {
  Id64 marketId;
  Id64 ids[MaxIds];
  // std::chrono::time_point is not pod.
  int64_t modified;
  More more;
};
static_assert(std::is_pod<ArchiveTradeBody>::value, "message-type must be pod");

struct SWIRLY_PACKED Msg {
  MsgType type;
  union SWIRLY_PACKED {
    CreateMarketBody createMarket;
    UpdateMarketBody updateMarket;
    CreateExecBody createExec;
    ArchiveTradeBody archiveTrade;
  };
};
static_assert(std::is_pod<Msg>::value, "message-type must be pod");
static_assert(sizeof(Msg) == 240, "unexpected message length");

using MsgPipe = Pipe<Msg>;

} // swirly

#endif // SWIRLY_ELM_MSG_HPP
