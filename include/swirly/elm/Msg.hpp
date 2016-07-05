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

#include <swirly/ash/Mnem.hpp>
#include <swirly/ash/Pipe.hpp>
#include <swirly/ash/Types.hpp>

namespace swirly {

/**
 * @addtogroup IO
 * @{
 */

enum class MsgType : int {
  Reset,
  CreateMarket,
  UpdateMarket,
  CreateExec,
  ArchiveOrder,
  ArchiveTrade
};

struct SWIRLY_PACKED CreateMarketBody {
  char mnem[MaxMnem];
  char display[MaxDisplay];
  char contr[MaxMnem];
  Jday settlDay;
  Jday expiryDay;
  MarketState state;
};
static_assert(std::is_pod<CreateMarketBody>::value, "message-type must be pod");

struct SWIRLY_PACKED UpdateMarketBody {
  char mnem[MaxMnem];
  char display[MaxDisplay];
  MarketState state;
};
static_assert(std::is_pod<UpdateMarketBody>::value, "message-type must be pod");

struct SWIRLY_PACKED CreateExecBody {
  char accnt[MaxMnem];
  char market[MaxMnem];
  char contr[MaxMnem];
  Jday settlDay;
  Iden id;
  char ref[MaxRef];
  Iden orderId;
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
  Iden matchId;
  LiqInd liqInd;
  char cpty[MaxMnem];
  Millis created;
  More more;
};
static_assert(std::is_pod<CreateExecBody>::value, "message-type must be pod");

constexpr std::size_t MaxIds{(sizeof(CreateExecBody) - MaxMnem - sizeof(Millis) - sizeof(More))
                             / sizeof(Iden)};
struct SWIRLY_PACKED ArchiveBody {
  char market[MaxMnem];
  Iden ids[MaxIds];
  Millis modified;
  More more;
};
static_assert(std::is_pod<ArchiveBody>::value, "message-type must be pod");

struct SWIRLY_PACKED Msg {
  MsgType type;
  union {
    CreateMarketBody createMarket;
    UpdateMarketBody updateMarket;
    CreateExecBody createExec;
    ArchiveBody archive;
  };
};
static_assert(std::is_pod<Msg>::value, "message-type must be pod");
static_assert(sizeof(Msg) == 248, "unexpected message length");

template <typename DerivedT>
struct MsgHandler {

  MsgHandler() noexcept = default;
  ~MsgHandler() noexcept = default;

  // Copy.
  MsgHandler(const MsgHandler&) noexcept = default;
  MsgHandler& operator=(const MsgHandler&) noexcept = default;

  // Move.
  MsgHandler(MsgHandler&&) noexcept = default;
  MsgHandler& operator=(MsgHandler&&) noexcept = default;

  void dispatch(const Msg& msg)
  {
    auto* derived = static_cast<DerivedT*>(this);
    switch (msg.type) {
    case MsgType::Reset:
      derived->reset();
      break;
    case MsgType::CreateMarket:
      derived->createMarket(msg.createMarket);
      break;
    case MsgType::UpdateMarket:
      derived->updateMarket(msg.updateMarket);
      break;
    case MsgType::CreateExec:
      derived->createExec(msg.createExec);
      break;
    case MsgType::ArchiveOrder:
      derived->archiveOrder(msg.archive);
      break;
    case MsgType::ArchiveTrade:
      derived->archiveTrade(msg.archive);
      break;
    }
  }
};

using MsgPipe = Pipe<Msg>;

/** @} */

} // swirly

#endif // SWIRLY_ELM_MSG_HPP
