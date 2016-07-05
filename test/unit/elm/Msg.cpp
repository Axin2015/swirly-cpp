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
#include <swirly/elm/Msg.hpp>

#include <swirly/tea/Test.hpp>

using namespace std;
using namespace swirly;

namespace {

struct Handler : MsgHandler<Handler> {

  int resetCalls{0};
  int createMarketCalls{0};
  int updateMarketCalls{0};
  int createExecCalls{0};
  int archiveOrderCalls{0};
  int archiveTradeCalls{0};

  void reset() { ++resetCalls; }
  void createMarket(const CreateMarketBody& body) { ++createMarketCalls; }
  void updateMarket(const UpdateMarketBody& body) { ++updateMarketCalls; }
  void createExec(const CreateExecBody& body) { ++createExecCalls; }
  void archiveOrder(const ArchiveBody& body) { ++archiveOrderCalls; }
  void archiveTrade(const ArchiveBody& body) { ++archiveTradeCalls; }
};

} // anonymous

SWIRLY_TEST_CASE(MsgHandler)
{
  Handler h;
  Msg m;
  memset(&m, 0, sizeof(m));

  SWIRLY_CHECK(h.resetCalls == 0);
  h.dispatch(m);
  SWIRLY_CHECK(h.resetCalls == 1);

  m.type = MsgType::Reset;
  SWIRLY_CHECK(h.resetCalls == 1);
  h.dispatch(m);
  SWIRLY_CHECK(h.resetCalls == 2);

  m.type = MsgType::CreateMarket;
  SWIRLY_CHECK(h.createMarketCalls == 0);
  h.dispatch(m);
  SWIRLY_CHECK(h.createMarketCalls == 1);

  m.type = MsgType::UpdateMarket;
  SWIRLY_CHECK(h.updateMarketCalls == 0);
  h.dispatch(m);
  SWIRLY_CHECK(h.updateMarketCalls == 1);

  m.type = MsgType::CreateExec;
  SWIRLY_CHECK(h.createExecCalls == 0);
  h.dispatch(m);
  SWIRLY_CHECK(h.createExecCalls == 1);

  m.type = MsgType::ArchiveOrder;
  SWIRLY_CHECK(h.archiveOrderCalls == 0);
  h.dispatch(m);
  SWIRLY_CHECK(h.archiveOrderCalls == 1);

  m.type = MsgType::ArchiveTrade;
  SWIRLY_CHECK(h.archiveTradeCalls == 0);
  h.dispatch(m);
  SWIRLY_CHECK(h.archiveTradeCalls == 1);
}
