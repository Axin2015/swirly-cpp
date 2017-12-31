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
#include "AsyncJourn.hpp"

#include <swirly/fin/Exec.hpp>
#include <swirly/fin/Journ.hpp>

#include <swirly/util/Log.hpp>

using namespace std;

namespace swirly {
namespace {
template <size_t StepN>
class MultiPart {
  public:
    MultiPart(AsyncJourn& journ, size_t total) noexcept
      : journ_{journ}
      , window_{total}
    {
    }
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

    size_t index() const noexcept { return window_.index(); }
    size_t size() const noexcept { return window_.size(); }
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
} // namespace

AsyncJourn::AsyncJourn(Journ& journ, size_t pipeCapacity)
  : pipe_{pipeCapacity}
  , thread_{worker, ref(pipe_), ref(journ)}
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

void AsyncJourn::archiveTrade(Id64 marketId, ArrayView<Id64> ids, Time modified)
{
    MultiPart<MaxIds> mp{*this, ids.size()};
    do {
        doArchiveTrade(marketId, makeArrayView(&ids[mp.index()], mp.size()), modified, mp.more());
    } while (mp.next());
}

void AsyncJourn::doReset()
{
    pipe_.write([](Msg& msg) { msg.type = MsgType::Reset; });
}

void AsyncJourn::doCreateMarket(Id64 id, Symbol instr, JDay settlDay, MarketState state)
{
    pipe_.write([id, &instr, settlDay, state](Msg& msg) {
        msg.type = MsgType::CreateMarket;
        auto& body = msg.createMarket;
        body.id = id;
        setCString(body.instr, instr);
        body.settlDay = settlDay;
        body.state = state;
    });
}

void AsyncJourn::doUpdateMarket(Id64 id, MarketState state)
{
    pipe_.write([&id, state](Msg& msg) {
        msg.type = MsgType::UpdateMarket;
        auto& body = msg.updateMarket;
        body.id = id;
        body.state = state;
    });
}

void AsyncJourn::doCreateExec(const Exec& exec, More more)
{
    pipe_.write([&exec, more](Msg& msg) {
        msg.type = MsgType::CreateExec;
        auto& body = msg.createExec;
        setCString(body.accnt, exec.accnt());
        body.marketId = exec.marketId();
        setCString(body.instr, exec.instr());
        body.settlDay = exec.settlDay();
        body.id = exec.id();
        body.orderId = exec.orderId();
        setCString(body.ref, exec.ref());
        body.state = exec.state();
        body.side = exec.side();
        body.lots = exec.lots();
        body.ticks = exec.ticks();
        body.resdLots = exec.resdLots();
        body.execLots = exec.execLots();
        body.execCost = exec.execCost();
        body.lastLots = exec.lastLots();
        body.lastTicks = exec.lastTicks();
        body.minLots = exec.minLots();
        body.matchId = exec.matchId();
        body.liqInd = exec.liqInd();
        setCString(body.cpty, exec.cpty());
        body.created = msSinceEpoch(exec.created());
        body.more = more;
    });
}

void AsyncJourn::doArchiveTrade(Id64 marketId, ArrayView<Id64> ids, Time modified, More more)
{
    assert(ids.size() <= MaxIds);
    pipe_.write([&marketId, ids, modified, more](Msg& msg) {
        msg.type = MsgType::ArchiveTrade;
        auto& body = msg.archiveTrade;
        body.marketId = marketId;
        // Cannot use copy and fill here because ArchiveBody is a packed struct:
        // auto it = copy(ids.begin(), ids.end(), begin(body.ids));
        // fill(it, end(body.ids), 0);
        size_t i{0};
        for (; i < ids.size(); ++i) {
            body.ids[i] = ids[i];
        }
        for (; i < MaxIds; ++i) {
            body.ids[i] = 0_id64;
        }
        body.modified = msSinceEpoch(modified);
        body.more = more;
    });
}

} // namespace swirly
