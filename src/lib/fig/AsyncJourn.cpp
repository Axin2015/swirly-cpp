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
#include <swirly/fig/AsyncJourn.hpp>

#include <swirly/elm/Exec.hpp>
#include <swirly/elm/Journ.hpp>

#include <swirly/ash/Log.hpp>

using namespace std;

namespace swirly {
namespace {
class MultiPart {
 public:
  explicit MultiPart(AsyncJourn& journ) noexcept : journ_{journ} {}
  ~MultiPart() noexcept
  {
    if (!done_) {
      try {
        journ_.reset();
      } catch (const exception& e) {
        SWIRLY_ERROR(logMsg() << "failed to reset multi-part: " << e.what());
      }
    }
  }
  void done() noexcept { done_ = true; }

 private:
  AsyncJourn& journ_;
  bool done_{false};
};

void worker(MsgPipe& pipe, Journ& journ)
{
  SWIRLY_NOTICE(logMsg() << "started async journal");
  auto fn = [&journ](const auto& msg) {
    try {
      journ.update(msg);
    } catch (const exception& e) {
      SWIRLY_ERROR(logMsg() << "failed to update journal: " << e.what());
    }
  };
  while (pipe.read(fn))
    ;
  SWIRLY_NOTICE(logMsg() << "stopped async journal");
}
} // anonymous

AsyncJourn::AsyncJourn(Journ& journ, size_t capacity)
  : pipe_{capacity}, thread_{worker, ref(pipe_), ref(journ)}
{
}

AsyncJourn::~AsyncJourn() noexcept
{
  try {
    pipe_.close();
  } catch (const exception& e) {
    SWIRLY_ERROR(logMsg() << "failed to close pipe: " << e.what());
  }
  try {
    thread_.join();
  } catch (const exception& e) {
    SWIRLY_ERROR(logMsg() << "failed to join thread: " << e.what());
  }
}

void AsyncJourn::createExec(ArrayView<ConstExecPtr> execs)
{
  if (execs.size() > 1) {
    doCreateExec(*execs.front(), More::Yes);
    MultiPart mp{*this};
    for (const auto& exec : makeArrayView(&execs[1], execs.size() - 2)) {
      doCreateExec(*exec, More::Yes);
    }
    doCreateExec(*execs.back(), More::No);
    mp.done();
  } else {
    doCreateExec(*execs.front(), More::No);
  }
}

void AsyncJourn::archiveOrder(Mnem market, ArrayView<Iden> ids, Millis modified)
{
  if (ids.size() > 1) {
    doArchiveOrder(market, ids.front(), modified, More::Yes);
    MultiPart mp{*this};
    for (const auto id : makeArrayView(&ids[1], ids.size() - 2)) {
      doArchiveOrder(market, id, modified, More::Yes);
    }
    doArchiveOrder(market, ids.back(), modified, More::No);
    mp.done();
  } else {
    doArchiveOrder(market, ids.front(), modified, More::No);
  }
}

void AsyncJourn::archiveTrade(Mnem market, ArrayView<Iden> ids, Millis modified)
{
  if (ids.size() > 1) {
    doArchiveTrade(market, ids.front(), modified, More::Yes);
    MultiPart mp{*this};
    for (const auto id : makeArrayView(&ids[1], ids.size() - 2)) {
      doArchiveTrade(market, id, modified, More::Yes);
    }
    doArchiveTrade(market, ids.back(), modified, More::No);
    mp.done();
  } else {
    doArchiveTrade(market, ids.front(), modified, More::No);
  }
}

void AsyncJourn::doReset()
{
  pipe_.write([](Msg& msg) { msg.type = MsgType::Reset; });
}

void AsyncJourn::doCreateMarket(Mnem mnem, string_view display, Mnem contr, Jday settlDay,
                                Jday expiryDay, MarketState state)
{
  pipe_.write([&mnem, &display, &contr, settlDay, expiryDay, state](Msg& msg) {
    msg.type = MsgType::CreateMarket;
    auto& body = msg.createMarket;
    setCString(body.mnem, mnem);
    setCString(body.display, display);
    setCString(body.contr, contr);
    body.settlDay = settlDay;
    body.expiryDay = expiryDay;
    body.state = state;
  });
}

void AsyncJourn::doUpdateMarket(Mnem mnem, string_view display, MarketState state)
{
  pipe_.write([&mnem, &display, state](Msg& msg) {
    msg.type = MsgType::UpdateMarket;
    auto& body = msg.updateMarket;
    setCString(body.mnem, mnem);
    setCString(body.display, display);
    body.state = state;
  });
}

void AsyncJourn::doCreateTrader(Mnem mnem, string_view display, string_view email)
{
  pipe_.write([&mnem, &display, &email](Msg& msg) {
    msg.type = MsgType::CreateTrader;
    auto& body = msg.createTrader;
    setCString(body.mnem, mnem);
    setCString(body.display, display);
    setCString(body.email, email);
  });
}

void AsyncJourn::doUpdateTrader(Mnem mnem, string_view display)
{
  pipe_.write([&mnem, &display](Msg& msg) {
    msg.type = MsgType::UpdateTrader;
    auto& body = msg.updateTrader;
    setCString(body.mnem, mnem);
    setCString(body.display, display);
  });
}

void AsyncJourn::doCreateExec(const Exec& exec, More more)
{
  pipe_.write([&exec, more](Msg& msg) {
    msg.type = MsgType::CreateExec;
    auto& body = msg.createExec;
    setCString(body.trader, exec.trader());
    setCString(body.market, exec.market());
    setCString(body.contr, exec.contr());
    body.settlDay = exec.settlDay();
    body.id = exec.id();
    setCString(body.ref, exec.ref());
    body.orderId = exec.orderId();
    body.state = exec.state();
    body.side = exec.side();
    body.lots = exec.lots();
    body.ticks = exec.ticks();
    body.resd = exec.resd();
    body.exec = exec.exec();
    body.cost = exec.cost();
    body.lastLots = exec.lastLots();
    body.lastTicks = exec.lastTicks();
    body.minLots = exec.minLots();
    body.matchId = exec.matchId();
    body.role = exec.role();
    setCString(body.cpty, exec.cpty());
    body.created = exec.created();
    body.more = more;
  });
}

void AsyncJourn::doArchiveOrder(Mnem market, Iden id, Millis modified, More more)
{
  pipe_.write([&market, id, modified, more](Msg& msg) {
    msg.type = MsgType::ArchiveOrder;
    auto& body = msg.archive;
    setCString(body.market, market);
    body.ids[0] = id;
    body.modified = modified;
    body.more = more;
  });
}

void AsyncJourn::doArchiveTrade(Mnem market, Iden id, Millis modified, More more)
{
  pipe_.write([&market, id, modified, more](Msg& msg) {
    msg.type = MsgType::ArchiveTrade;
    auto& body = msg.archive;
    setCString(body.market, market);
    body.ids[0] = id;
    body.modified = modified;
    body.more = more;
  });
}

} // swirly
