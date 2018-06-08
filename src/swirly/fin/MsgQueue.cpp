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

void MsgQueue::archive_trade(Id64 market_id, ArrayView<Id64> ids, Time modified)
{
    detail::Range<MaxIds> r{ids.size()};
    if (mq_.reserve() < r.steps()) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
    do {
        do_archive_trade(market_id, make_array_view(&ids[r.step_offset()], r.step_size()),
                         modified);
    } while (r.next());
}

void MsgQueue::do_create_market(Id64 id, Symbol instr, JDay settl_day, MarketState state)
{
    const auto fn = [ id, &instr, settl_day, state ](Msg & msg) noexcept
    {
        msg.type = MsgType::CreateMarket;
        auto& body = msg.create_market;
        body.id = id;
        pstrcpy<'\0'>(body.instr, instr);
        body.settl_day = settl_day;
        body.state = state;
    };
    if (!mq_.post(fn)) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
}

void MsgQueue::do_update_market(Id64 id, MarketState state)
{
    const auto fn = [&id, state ](Msg & msg) noexcept
    {
        msg.type = MsgType::UpdateMarket;
        auto& body = msg.update_market;
        body.id = id;
        body.state = state;
    };
    if (!mq_.post(fn)) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
}

void MsgQueue::do_create_exec(const Exec& exec)
{
    const auto fn = [&exec](Msg & msg) noexcept
    {
        msg.type = MsgType::CreateExec;
        auto& body = msg.create_exec;
        pstrcpy<'\0'>(body.accnt, exec.accnt());
        body.market_id = exec.market_id();
        pstrcpy<'\0'>(body.instr, exec.instr());
        body.settl_day = exec.settl_day();
        body.id = exec.id();
        body.order_id = exec.order_id();
        pstrcpy<'\0'>(body.ref, exec.ref());
        body.state = exec.state();
        body.side = exec.side();
        body.lots = exec.lots();
        body.ticks = exec.ticks();
        body.resd_lots = exec.resd_lots();
        body.exec_lots = exec.exec_lots();
        body.exec_cost = exec.exec_cost();
        body.last_lots = exec.last_lots();
        body.last_ticks = exec.last_ticks();
        body.min_lots = exec.min_lots();
        body.match_id = exec.match_id();
        body.posn_lots = exec.posn_lots();
        body.posn_cost = exec.posn_cost();
        body.liq_ind = exec.liq_ind();
        pstrcpy<'\0'>(body.cpty, exec.cpty());
        body.created = ms_since_epoch(exec.created());
    };
    if (!mq_.post(fn)) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
}

void MsgQueue::create_exec(ArrayView<ConstExecPtr> execs)
{
    if (mq_.reserve() < execs.size()) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
    for (const auto& exec : execs) {
        do_create_exec(*exec);
    }
}

void MsgQueue::do_archive_trade(Id64 market_id, ArrayView<Id64> ids, Time modified)
{
    assert(ids.size() <= MaxIds);
    const auto fn = [&market_id, ids, modified ](Msg & msg) noexcept
    {
        msg.type = MsgType::ArchiveTrade;
        auto& body = msg.archive_trade;
        body.market_id = market_id;
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
        body.modified = ms_since_epoch(modified);
    };
    if (!mq_.post(fn)) {
        throw std::runtime_error{"insufficient queue capacity"};
    }
}

} // namespace fin
} // namespace swirly
