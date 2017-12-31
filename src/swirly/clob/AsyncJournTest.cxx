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
#include <swirly/fin/MarketId.hpp>
#include <swirly/fin/Msg.hpp>

#include <swirly/util/Date.hpp>

#include <swirly/sys/Time.hpp>

#include <swirly/unit/Test.hpp>

#include <algorithm>
#include <queue>

using namespace std;
using namespace swirly;

namespace {

constexpr auto Today = ymdToJd(2014, 2, 11);
constexpr auto SettlDay = Today + 2_jd;
constexpr auto MarketId = toMarketId(1_id32, SettlDay);

constexpr auto Now = jdToTime(Today);

struct JournState {
    int reset{0};
    int createMarket{0};
    int updateMarket{0};
    int createExec{0};
    int archiveTrade{0};
    int total{0};
};

struct TestJourn : Journ {
    bool pop(Msg& msg)
    {
        unique_lock<mutex> lock{mutex_};
        auto now = chrono::system_clock::now();
        auto pred = [this]() { return !this->msgs_.empty(); };
        if (!notEmpty_.wait_until(lock, now + chrono::seconds(2), pred)) {
            // Timeout.
            return false;
        }
        msg = msgs_.front();
        msgs_.pop();
        return true;
    }

  protected:
    void doUpdate(const Msg& msg) override
    {
        {
            lock_guard<mutex> lock{mutex_};
            msgs_.push(msg);
        }
        notEmpty_.notify_one();
    }

  private:
    mutex mutex_;
    condition_variable notEmpty_;
    queue<Msg> msgs_;
};

struct AsyncJournFixture {
    AsyncJournFixture()
      : asyncJourn{journ, 1 << 10}
    {
    }
    TestJourn journ;
    AsyncJourn asyncJourn;
};

} // namespace

SWIRLY_TEST_CASE(AsyncWindow)
{
    {
        detail::AsyncWindow<1> aw{3};
        // 1.
        SWIRLY_CHECK(aw.index() == 0);
        SWIRLY_CHECK(aw.size() == 1);
        SWIRLY_CHECK(aw.more() == More::Yes);
        SWIRLY_CHECK(aw.next());
        SWIRLY_CHECK(!aw.done());
        // 2.
        SWIRLY_CHECK(aw.index() == 1);
        SWIRLY_CHECK(aw.size() == 1);
        SWIRLY_CHECK(aw.more() == More::Yes);
        SWIRLY_CHECK(aw.next());
        SWIRLY_CHECK(!aw.done());
        // 3.
        SWIRLY_CHECK(aw.index() == 2);
        SWIRLY_CHECK(aw.size() == 1);
        SWIRLY_CHECK(aw.more() == More::No);
        SWIRLY_CHECK(!aw.next());
        SWIRLY_CHECK(aw.done());
    }
    {
        detail::AsyncWindow<8> aw{4};
        // 0-3.
        SWIRLY_CHECK(aw.index() == 0);
        SWIRLY_CHECK(aw.size() == 4);
        SWIRLY_CHECK(aw.more() == More::No);
        SWIRLY_CHECK(!aw.next());
        SWIRLY_CHECK(aw.done());
    }
    {
        detail::AsyncWindow<8> aw{16};
        // 0-7.
        SWIRLY_CHECK(aw.index() == 0);
        SWIRLY_CHECK(aw.size() == 8);
        SWIRLY_CHECK(aw.more() == More::Yes);
        SWIRLY_CHECK(aw.next());
        SWIRLY_CHECK(!aw.done());
        // 8-15
        SWIRLY_CHECK(aw.index() == 8);
        SWIRLY_CHECK(aw.size() == 8);
        SWIRLY_CHECK(aw.more() == More::No);
        SWIRLY_CHECK(!aw.next());
        SWIRLY_CHECK(aw.done());
    }
    {
        detail::AsyncWindow<8> aw{20};
        // 0-7.
        SWIRLY_CHECK(aw.index() == 0);
        SWIRLY_CHECK(aw.size() == 8);
        SWIRLY_CHECK(aw.more() == More::Yes);
        SWIRLY_CHECK(aw.next());
        SWIRLY_CHECK(!aw.done());
        // 8-15
        SWIRLY_CHECK(aw.index() == 8);
        SWIRLY_CHECK(aw.size() == 8);
        SWIRLY_CHECK(aw.more() == More::Yes);
        SWIRLY_CHECK(aw.next());
        SWIRLY_CHECK(!aw.done());
        // 16-19.
        SWIRLY_CHECK(aw.index() == 16);
        SWIRLY_CHECK(aw.size() == 4);
        SWIRLY_CHECK(aw.more() == More::No);
        SWIRLY_CHECK(!aw.next());
        SWIRLY_CHECK(aw.done());
    }
}

SWIRLY_FIXTURE_TEST_CASE(AsyncJournCreateMarket, AsyncJournFixture)
{
    asyncJourn.createMarket(MarketId, "EURUSD"_sv, SettlDay, 0x1);

    Msg msg;
    SWIRLY_CHECK(journ.pop(msg));
    SWIRLY_CHECK(msg.type == MsgType::CreateMarket);
    const auto& body = msg.createMarket;

    SWIRLY_CHECK(body.id == MarketId);
    SWIRLY_CHECK(strncmp(body.instr, "EURUSD", sizeof(body.instr)) == 0);
    SWIRLY_CHECK(body.settlDay == SettlDay);
    SWIRLY_CHECK(body.state == 0x1);
}

SWIRLY_FIXTURE_TEST_CASE(AsyncJournUpdateMarket, AsyncJournFixture)
{
    asyncJourn.updateMarket(MarketId, 0x1);

    Msg msg;
    SWIRLY_CHECK(journ.pop(msg));
    SWIRLY_CHECK(msg.type == MsgType::UpdateMarket);
    const auto& body = msg.updateMarket;

    SWIRLY_CHECK(body.id == MarketId);
    SWIRLY_CHECK(body.state == 0x1);
}

SWIRLY_FIXTURE_TEST_CASE(AsyncJournCreateExec, AsyncJournFixture)
{
    ConstExecPtr execs[2];
    execs[0]
        = makeIntrusive<Exec>("MARAYL"_sv, MarketId, "EURUSD"_sv, SettlDay, 1_id64, 2_id64,
                              "REF"_sv, State::New, Side::Buy, 10_lts, 12345_tks, 10_lts, 0_lts,
                              0_cst, 0_lts, 0_tks, 1_lts, 0_id64, LiqInd::None, Symbol{}, Now);
    execs[1] = makeIntrusive<Exec>("MARAYL"_sv, MarketId, "EURUSD"_sv, SettlDay, 3_id64, 2_id64,
                                   "REF"_sv, State::Trade, Side::Buy, 10_lts, 12345_tks, 5_lts,
                                   5_lts, 61725_cst, 5_lts, 12345_tks, 1_lts, 4_id64, LiqInd::Maker,
                                   "GOSAYL"_sv, Now + 1ms);
    asyncJourn.createExec(execs);
    {
        Msg msg;
        SWIRLY_CHECK(journ.pop(msg));
        SWIRLY_CHECK(msg.type == MsgType::CreateExec);
        const auto& body = msg.createExec;

        SWIRLY_CHECK(strncmp(body.accnt, "MARAYL", sizeof(body.accnt)) == 0);
        SWIRLY_CHECK(body.marketId == MarketId);
        SWIRLY_CHECK(strncmp(body.instr, "EURUSD", sizeof(body.instr)) == 0);
        SWIRLY_CHECK(body.settlDay == SettlDay);
        SWIRLY_CHECK(body.id == 1_id64);
        SWIRLY_CHECK(body.orderId == 2_id64);
        SWIRLY_CHECK(strncmp(body.ref, "REF", sizeof(body.ref)) == 0);
        SWIRLY_CHECK(body.state == State::New);
        SWIRLY_CHECK(body.side == Side::Buy);
        SWIRLY_CHECK(body.lots == 10_lts);
        SWIRLY_CHECK(body.ticks == 12345_tks);
        SWIRLY_CHECK(body.resdLots == 10_lts);
        SWIRLY_CHECK(body.execLots == 0_lts);
        SWIRLY_CHECK(body.execCost == 0_cst);
        SWIRLY_CHECK(body.lastLots == 0_lts);
        SWIRLY_CHECK(body.lastTicks == 0_tks);
        SWIRLY_CHECK(body.minLots == 1_lts);
        SWIRLY_CHECK(body.matchId == 0_id64);
        SWIRLY_CHECK(body.liqInd == LiqInd::None);
        SWIRLY_CHECK(strncmp(body.cpty, "", sizeof(body.cpty)) == 0);
        SWIRLY_CHECK(body.created == msSinceEpoch(Now));
    }
    {
        Msg msg;
        SWIRLY_CHECK(journ.pop(msg));
        SWIRLY_CHECK(msg.type == MsgType::CreateExec);
        const auto& body = msg.createExec;

        SWIRLY_CHECK(body.marketId == MarketId);
        SWIRLY_CHECK(strncmp(body.instr, "EURUSD", sizeof(body.instr)) == 0);
        SWIRLY_CHECK(body.settlDay == SettlDay);
        SWIRLY_CHECK(body.id == 3_id64);
        SWIRLY_CHECK(body.orderId == 2_id64);
        SWIRLY_CHECK(strncmp(body.accnt, "MARAYL", sizeof(body.accnt)) == 0);
        SWIRLY_CHECK(strncmp(body.ref, "REF", sizeof(body.ref)) == 0);
        SWIRLY_CHECK(body.state == State::Trade);
        SWIRLY_CHECK(body.side == Side::Buy);
        SWIRLY_CHECK(body.lots == 10_lts);
        SWIRLY_CHECK(body.ticks == 12345_tks);
        SWIRLY_CHECK(body.resdLots == 5_lts);
        SWIRLY_CHECK(body.execLots == 5_lts);
        SWIRLY_CHECK(body.execCost == 61725_cst);
        SWIRLY_CHECK(body.lastLots == 5_lts);
        SWIRLY_CHECK(body.lastTicks == 12345_tks);
        SWIRLY_CHECK(body.minLots == 1_lts);
        SWIRLY_CHECK(body.matchId == 4_id64);
        SWIRLY_CHECK(body.liqInd == LiqInd::Maker);
        SWIRLY_CHECK(strncmp(body.cpty, "GOSAYL", sizeof(body.cpty)) == 0);
        SWIRLY_CHECK(body.created == msSinceEpoch(Now + 1ms));
    }
}

SWIRLY_FIXTURE_TEST_CASE(AsyncJournArchiveTrade, AsyncJournFixture)
{
    vector<Id64> ids;
    ids.reserve(101);

    Id64 id{};
    generate_n(back_insert_iterator<decltype(ids)>(ids), ids.capacity(), [&id]() { return ++id; });
    asyncJourn.archiveTrade(MarketId, ids, Now);

    auto it = ids.begin();
    while (it != ids.end()) {
        Msg msg;
        SWIRLY_CHECK(journ.pop(msg));
        SWIRLY_CHECK(msg.type == MsgType::ArchiveTrade);
        const auto& body = msg.archiveTrade;
        SWIRLY_CHECK(body.marketId == MarketId);
        SWIRLY_CHECK(body.modified == msSinceEpoch(Now));
        for (const auto id : body.ids) {
            if (it != ids.end()) {
                SWIRLY_CHECK(id == *it++);
            } else {
                SWIRLY_CHECK(id == 0_id64);
            }
        }
    }
}
