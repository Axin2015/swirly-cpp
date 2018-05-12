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
#ifndef SWIRLY_FIN_MSG_HPP
#define SWIRLY_FIN_MSG_HPP

#include <swirly/fin/Types.hpp>

#include <swirly/app/MemQueue.hpp>

#include <swirly/util/BasicTypes.hpp>
#include <swirly/util/Date.hpp>
#include <swirly/util/Symbol.hpp>

namespace swirly {
inline namespace fin {

enum class MsgType : int { CreateMarket, UpdateMarket, CreateExec, ArchiveTrade };

struct SWIRLY_PACKED CreateMarket {
    Id64 id;
    char instr[MaxSymbol];
    JDay settlDay;
    MarketState state;
};
static_assert(std::is_pod_v<CreateMarket>);

struct SWIRLY_PACKED UpdateMarket {
    Id64 id;
    MarketState state;
};
static_assert(std::is_pod_v<UpdateMarket>);

struct SWIRLY_PACKED CreateExec {
    char accnt[MaxSymbol];
    Id64 marketId;
    char instr[MaxSymbol];
    JDay settlDay;
    Id64 id;
    Id64 orderId;
    char ref[MaxRef];
    State state;
    Side side;
    Lots lots;
    Ticks ticks;
    Lots resdLots;
    Lots execLots;
    Cost execCost;
    Lots lastLots;
    Ticks lastTicks;
    Lots minLots;
    Id64 matchId;
    LiqInd liqInd;
    char cpty[MaxSymbol];
    // std::chrono::time_point is not pod.
    int64_t created;
};
static_assert(std::is_pod_v<CreateExec>);

constexpr std::size_t MaxIds{(sizeof(CreateExec) - sizeof(Id64) - sizeof(int64_t)) / sizeof(Id64)};
struct SWIRLY_PACKED ArchiveTrade {
    Id64 marketId;
    Id64 ids[MaxIds];
    // std::chrono::time_point is not pod.
    int64_t modified;
};
static_assert(std::is_pod_v<ArchiveTrade>);

struct SWIRLY_PACKED Msg {
    MsgType type;
    union SWIRLY_PACKED {
        CreateMarket createMarket;
        UpdateMarket updateMarket;
        CreateExec createExec;
        ArchiveTrade archiveTrade;
    };
};
static_assert(std::is_pod_v<Msg>);
static_assert(sizeof(Msg) == 236, "must be specific size");

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_MSG_HPP
