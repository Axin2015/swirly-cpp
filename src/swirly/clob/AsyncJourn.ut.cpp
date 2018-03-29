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

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <queue>

namespace swirly {
inline namespace clob {
inline namespace clob {
std::ostream& operator<<(std::ostream& os, More more)
{
    switch (more) {
    case More::Yes:
        os << "Yes";
        break;
    case More::No:
        os << "No";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, MsgType type)
{
    switch (type) {
    case MsgType::Reset:
        os << "Reset";
        break;
    case MsgType::CreateMarket:
        os << "CreateMarket";
        break;
    case MsgType::UpdateMarket:
        os << "UpdateMarket";
        break;
    case MsgType::CreateExec:
        os << "CreateExec";
        break;
    case MsgType::ArchiveTrade:
        os << "ArchiveTrade";
        break;
    }
    return os;
}
} // namespace clob
} // namespace clob
} // namespace swirly

using namespace std;
using namespace swirly;

namespace {

constexpr auto Today = ymdToJd(2014, 3, 11);
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

BOOST_AUTO_TEST_SUITE(AsyncJournSuite)

BOOST_AUTO_TEST_CASE(AsyncWindowCase)
{
    {
        detail::AsyncWindow<1> aw{3};
        // 1.
        BOOST_TEST(aw.index() == 0U);
        BOOST_TEST(aw.size() == 1U);
        BOOST_TEST(aw.more() == More::Yes);
        BOOST_TEST(aw.next());
        BOOST_TEST(!aw.done());
        // 2.
        BOOST_TEST(aw.index() == 1U);
        BOOST_TEST(aw.size() == 1U);
        BOOST_TEST(aw.more() == More::Yes);
        BOOST_TEST(aw.next());
        BOOST_TEST(!aw.done());
        // 3.
        BOOST_TEST(aw.index() == 2U);
        BOOST_TEST(aw.size() == 1U);
        BOOST_TEST(aw.more() == More::No);
        BOOST_TEST(!aw.next());
        BOOST_TEST(aw.done());
    }
    {
        detail::AsyncWindow<8> aw{4};
        // 0-3.
        BOOST_TEST(aw.index() == 0U);
        BOOST_TEST(aw.size() == 4U);
        BOOST_TEST(aw.more() == More::No);
        BOOST_TEST(!aw.next());
        BOOST_TEST(aw.done());
    }
    {
        detail::AsyncWindow<8> aw{16};
        // 0-7.
        BOOST_TEST(aw.index() == 0U);
        BOOST_TEST(aw.size() == 8U);
        BOOST_TEST(aw.more() == More::Yes);
        BOOST_TEST(aw.next());
        BOOST_TEST(!aw.done());
        // 8-15
        BOOST_TEST(aw.index() == 8U);
        BOOST_TEST(aw.size() == 8U);
        BOOST_TEST(aw.more() == More::No);
        BOOST_TEST(!aw.next());
        BOOST_TEST(aw.done());
    }
    {
        detail::AsyncWindow<8> aw{20};
        // 0-7.
        BOOST_TEST(aw.index() == 0U);
        BOOST_TEST(aw.size() == 8U);
        BOOST_TEST(aw.more() == More::Yes);
        BOOST_TEST(aw.next());
        BOOST_TEST(!aw.done());
        // 8-15
        BOOST_TEST(aw.index() == 8U);
        BOOST_TEST(aw.size() == 8U);
        BOOST_TEST(aw.more() == More::Yes);
        BOOST_TEST(aw.next());
        BOOST_TEST(!aw.done());
        // 16-19.
        BOOST_TEST(aw.index() == 16U);
        BOOST_TEST(aw.size() == 4U);
        BOOST_TEST(aw.more() == More::No);
        BOOST_TEST(!aw.next());
        BOOST_TEST(aw.done());
    }
}

BOOST_FIXTURE_TEST_CASE(AsyncJournCreateMarket, AsyncJournFixture)
{
    asyncJourn.createMarket(MarketId, "EURUSD"sv, SettlDay, 0x1);

    Msg msg;
    BOOST_TEST(journ.pop(msg));
    BOOST_CHECK_EQUAL(msg.type, MsgType::CreateMarket);
    const auto& body = msg.createMarket;

    BOOST_CHECK_EQUAL(body.id, MarketId);
    BOOST_CHECK_EQUAL(strncmp(body.instr, "EURUSD", sizeof(body.instr)), 0);
    BOOST_CHECK_EQUAL(body.settlDay, SettlDay);
    BOOST_CHECK_EQUAL(body.state, 0x1U);
}

BOOST_FIXTURE_TEST_CASE(AsyncJournUpdateMarket, AsyncJournFixture)
{
    asyncJourn.updateMarket(MarketId, 0x1);

    Msg msg;
    BOOST_TEST(journ.pop(msg));
    BOOST_CHECK_EQUAL(msg.type, MsgType::UpdateMarket);
    const auto& body = msg.updateMarket;

    BOOST_CHECK_EQUAL(body.id, MarketId);
    BOOST_CHECK_EQUAL(body.state, 0x1U);
}

BOOST_FIXTURE_TEST_CASE(AsyncJournCreateExec, AsyncJournFixture)
{
    ConstExecPtr execs[2];
    execs[0] = makeIntrusive<Exec>("MARAYL"sv, MarketId, "EURUSD"sv, SettlDay, 1_id64, 2_id64,
                                   "REF"sv, State::New, Side::Buy, 10_lts, 12345_tks, 10_lts, 0_lts,
                                   0_cst, 0_lts, 0_tks, 1_lts, 0_id64, LiqInd::None, Symbol{}, Now);
    execs[1] = makeIntrusive<Exec>("MARAYL"sv, MarketId, "EURUSD"sv, SettlDay, 3_id64, 2_id64,
                                   "REF"sv, State::Trade, Side::Buy, 10_lts, 12345_tks, 5_lts,
                                   5_lts, 61725_cst, 5_lts, 12345_tks, 1_lts, 4_id64, LiqInd::Maker,
                                   "GOSAYL"sv, Now + 1ms);
    asyncJourn.createExec(execs);
    {
        Msg msg;
        BOOST_TEST(journ.pop(msg));
        BOOST_CHECK_EQUAL(msg.type, MsgType::CreateExec);
        const auto& body = msg.createExec;

        BOOST_CHECK_EQUAL(strncmp(body.accnt, "MARAYL", sizeof(body.accnt)), 0);
        BOOST_CHECK_EQUAL(body.marketId, MarketId);
        BOOST_CHECK_EQUAL(strncmp(body.instr, "EURUSD", sizeof(body.instr)), 0);
        BOOST_CHECK_EQUAL(body.settlDay, SettlDay);
        BOOST_CHECK_EQUAL(body.id, 1_id64);
        BOOST_CHECK_EQUAL(body.orderId, 2_id64);
        BOOST_CHECK_EQUAL(strncmp(body.ref, "REF", sizeof(body.ref)), 0);
        BOOST_CHECK_EQUAL(body.state, State::New);
        BOOST_CHECK_EQUAL(body.side, Side::Buy);
        BOOST_CHECK_EQUAL(body.lots, 10_lts);
        BOOST_CHECK_EQUAL(body.ticks, 12345_tks);
        BOOST_CHECK_EQUAL(body.resdLots, 10_lts);
        BOOST_CHECK_EQUAL(body.execLots, 0_lts);
        BOOST_CHECK_EQUAL(body.execCost, 0_cst);
        BOOST_CHECK_EQUAL(body.lastLots, 0_lts);
        BOOST_CHECK_EQUAL(body.lastTicks, 0_tks);
        BOOST_CHECK_EQUAL(body.minLots, 1_lts);
        BOOST_CHECK_EQUAL(body.matchId, 0_id64);
        BOOST_CHECK_EQUAL(body.liqInd, LiqInd::None);
        BOOST_CHECK_EQUAL(strncmp(body.cpty, "", sizeof(body.cpty)), 0);
        BOOST_CHECK_EQUAL(body.created, msSinceEpoch(Now));
    }
    {
        Msg msg;
        BOOST_TEST(journ.pop(msg));
        BOOST_CHECK_EQUAL(msg.type, MsgType::CreateExec);
        const auto& body = msg.createExec;

        BOOST_CHECK_EQUAL(body.marketId, MarketId);
        BOOST_CHECK_EQUAL(strncmp(body.instr, "EURUSD", sizeof(body.instr)), 0);
        BOOST_CHECK_EQUAL(body.settlDay, SettlDay);
        BOOST_CHECK_EQUAL(body.id, 3_id64);
        BOOST_CHECK_EQUAL(body.orderId, 2_id64);
        BOOST_CHECK_EQUAL(strncmp(body.accnt, "MARAYL", sizeof(body.accnt)), 0);
        BOOST_CHECK_EQUAL(strncmp(body.ref, "REF", sizeof(body.ref)), 0);
        BOOST_CHECK_EQUAL(body.state, State::Trade);
        BOOST_CHECK_EQUAL(body.side, Side::Buy);
        BOOST_CHECK_EQUAL(body.lots, 10_lts);
        BOOST_CHECK_EQUAL(body.ticks, 12345_tks);
        BOOST_CHECK_EQUAL(body.resdLots, 5_lts);
        BOOST_CHECK_EQUAL(body.execLots, 5_lts);
        BOOST_CHECK_EQUAL(body.execCost, 61725_cst);
        BOOST_CHECK_EQUAL(body.lastLots, 5_lts);
        BOOST_CHECK_EQUAL(body.lastTicks, 12345_tks);
        BOOST_CHECK_EQUAL(body.minLots, 1_lts);
        BOOST_CHECK_EQUAL(body.matchId, 4_id64);
        BOOST_CHECK_EQUAL(body.liqInd, LiqInd::Maker);
        BOOST_CHECK_EQUAL(strncmp(body.cpty, "GOSAYL", sizeof(body.cpty)), 0);
        BOOST_CHECK_EQUAL(body.created, msSinceEpoch(Now + 1ms));
    }
}

BOOST_FIXTURE_TEST_CASE(AsyncJournArchiveTrade, AsyncJournFixture)
{
    vector<Id64> ids;
    ids.reserve(101);

    Id64 id{};
    generate_n(back_insert_iterator<decltype(ids)>(ids), ids.capacity(), [&id]() { return ++id; });
    asyncJourn.archiveTrade(MarketId, ids, Now);

    auto it = ids.begin();
    while (it != ids.end()) {
        Msg msg;
        BOOST_TEST(journ.pop(msg));
        BOOST_CHECK_EQUAL(msg.type, MsgType::ArchiveTrade);
        const auto& body = msg.archiveTrade;
        BOOST_CHECK_EQUAL(body.marketId, MarketId);
        BOOST_CHECK_EQUAL(body.modified, msSinceEpoch(Now));
        for (const auto id : body.ids) {
            if (it != ids.end()) {
                BOOST_CHECK_EQUAL(id, *it++);
            } else {
                BOOST_CHECK_EQUAL(id, 0_id64);
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
