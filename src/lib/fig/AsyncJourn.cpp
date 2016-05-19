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
template <std::size_t StepN>
class MultiPart {
 public:
  MultiPart(AsyncJourn& journ, std::size_t total) noexcept : journ_{journ}, window_{total} {}
  ~MultiPart() noexcept
  {
    if (!done()) {
      try {
        journ_.reset();
      } catch (const exception& e) {
        SWIRLY_ERROR(logMsg() << "failed to reset multi-part: " << e.what());
      }
    }
  }

  // Copy.
  MultiPart(const MultiPart&) = delete;
  MultiPart& operator=(const MultiPart&) = delete;

  // Move.
  MultiPart(MultiPart&&) = delete;
  MultiPart& operator=(MultiPart&&) = delete;

  std::size_t index() const noexcept { return window_.index(); }
  std::size_t size() const noexcept { return window_.size(); }
  More more() const noexcept { return window_.more(); }
  bool done() const noexcept { return window_.done(); }
  bool next() noexcept { return window_.next(); }

 private:
  AsyncJourn& journ_;
  detail::AsyncWindow<StepN> window_;
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
  MultiPart<1> mp{*this, execs.size()};
  do {
    doCreateExec(*execs[mp.index()], mp.more());
  } while (mp.next());
}

void AsyncJourn::archiveOrder(Mnem market, ArrayView<Iden> ids, Millis modified)
{
  MultiPart<MaxIds> mp{*this, ids.size()};
  do {
    doArchiveOrder(market, makeArrayView(&ids[mp.index()], mp.size()), modified, mp.more());
  } while (mp.next());
}

void AsyncJourn::archiveTrade(Mnem market, ArrayView<Iden> ids, Millis modified)
{
  MultiPart<MaxIds> mp{*this, ids.size()};
  do {
    doArchiveTrade(market, makeArrayView(&ids[mp.index()], mp.size()), modified, mp.more());
  } while (mp.next());
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

void AsyncJourn::doArchiveOrder(Mnem market, ArrayView<Iden> ids, Millis modified, More more)
{
  assert(ids.size() <= MaxIds);
  pipe_.write([&market, ids, modified, more](Msg& msg) {
    msg.type = MsgType::ArchiveOrder;
    auto& body = msg.archive;
    setCString(body.market, market);
    // Cannot use copy and fill here because ArchiveBody is a packed struct:
    // auto it = copy(ids.begin(), ids.end(), begin(body.ids));
    // fill(it, end(body.ids), 0);
    size_t i{0};
    for (; i < ids.size(); ++i) {
      body.ids[i] = ids[i];
    }
    for (; i < MaxIds; ++i) {
      body.ids[i] = 0_id;
    }
    body.modified = modified;
    body.more = more;
  });
}

void AsyncJourn::doArchiveTrade(Mnem market, ArrayView<Iden> ids, Millis modified, More more)
{
  assert(ids.size() <= MaxIds);
  pipe_.write([&market, ids, modified, more](Msg& msg) {
    msg.type = MsgType::ArchiveTrade;
    auto& body = msg.archive;
    setCString(body.market, market);
    // Cannot use copy and fill here because ArchiveBody is a packed struct:
    // auto it = copy(ids.begin(), ids.end(), begin(body.ids));
    // fill(it, end(body.ids), 0);
    size_t i{0};
    for (; i < ids.size(); ++i) {
      body.ids[i] = ids[i];
    }
    for (; i < MaxIds; ++i) {
      body.ids[i] = 0_id;
    }
    body.modified = modified;
    body.more = more;
  });
}

} // swirly
