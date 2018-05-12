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
#include "MsgQueue.hpp"

#include <swirly/fin/Exec.hpp>
#include <swirly/fin/Journ.hpp>

#include <swirly/util/Log.hpp>

#include <thread>

namespace swirly {
inline namespace fin {
using namespace std;

MsgQueue::~MsgQueue() = default;

void MsgQueue::archiveTrade(Id64 marketId, ArrayView<Id64> ids, Time modified)
{
    detail::Range<MaxIds> r{ids.size()};
    if (mq_.reserve() < r.steps()) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
    do {
        doArchiveTrade(marketId, makeArrayView(&ids[r.stepOffset()], r.stepSize()), modified);
    } while (r.next());
}

void MsgQueue::doCreateMarket(Id64 id, Symbol instr, JDay settlDay, MarketState state)
{
    const auto fn = [ id, &instr, settlDay, state ](Msg & msg) noexcept
    {
        msg.type = MsgType::CreateMarket;
        auto& body = msg.createMarket;
        body.id = id;
        setCString(body.instr, instr);
        body.settlDay = settlDay;
        body.state = state;
    };
    if (!mq_.post(fn)) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
}

void MsgQueue::doUpdateMarket(Id64 id, MarketState state)
{
    const auto fn = [&id, state ](Msg & msg) noexcept
    {
        msg.type = MsgType::UpdateMarket;
        auto& body = msg.updateMarket;
        body.id = id;
        body.state = state;
    };
    if (!mq_.post(fn)) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
}

void MsgQueue::doCreateExec(const Exec& exec)
{
    const auto fn = [&exec](Msg & msg) noexcept
    {
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
    };
    if (!mq_.post(fn)) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
}

void MsgQueue::createExec(ArrayView<ConstExecPtr> execs)
{
    if (mq_.reserve() < execs.size()) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
    for (const auto& exec : execs) {
        doCreateExec(*exec);
    }
}

void MsgQueue::doArchiveTrade(Id64 marketId, ArrayView<Id64> ids, Time modified)
{
    assert(ids.size() <= MaxIds);
    const auto fn = [&marketId, ids, modified ](Msg & msg) noexcept
    {
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
    };
    if (!mq_.post(fn)) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
}

} // namespace fin
} // namespace swirly
