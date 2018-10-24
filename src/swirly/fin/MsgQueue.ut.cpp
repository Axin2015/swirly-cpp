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
#include <swirly/fin/MarketId.hpp>
#include <swirly/fin/Msg.hpp>

#include <swirly/util/Date.hpp>

#include <swirly/util/Time.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <queue>

namespace swirly {
inline namespace fin {
std::ostream& operator<<(std::ostream& os, MsgType type)
{
    switch (type) {
    case MsgType::CreateMarket:
        os << "Create_market";
        break;
    case MsgType::UpdateMarket:
        os << "Update_market";
        break;
    case MsgType::CreateExec:
        os << "Create_exec";
        break;
    case MsgType::ArchiveTrade:
        os << "Archive_trade";
        break;
    }
    return os;
}
} // namespace fin
} // namespace swirly

using namespace std;
using namespace swirly;

namespace {

constexpr auto Today = ymd_to_jd(2014, 3, 11);
constexpr auto SettlDay = Today + 2_jd;
constexpr auto MarketId = to_market_id(1_id32, SettlDay);

constexpr auto Now = jd_to_time(Today);

struct JournState {
    int reset{0};
    int create_market{0};
    int update_market{0};
    int create_exec{0};
    int archive_trade{0};
    int total{0};
};

struct Fixture {
    MsgQueue mq{1 << 10};
};

} // namespace

BOOST_AUTO_TEST_SUITE(MsgQueueSuite)

BOOST_AUTO_TEST_CASE(RangeCase)
{
    BOOST_TEST(fin::detail::Range<8>{}.steps() == 0U);
    {
        fin::detail::Range<1> c{3};
        BOOST_TEST(c.steps() == 3U);
        // 1.
        BOOST_TEST(c.step_offset() == 0U);
        BOOST_TEST(c.step_size() == 1U);
        BOOST_TEST(c.next());
        BOOST_TEST(!c.done());
        // 2.
        BOOST_TEST(c.step_offset() == 1U);
        BOOST_TEST(c.step_size() == 1U);
        BOOST_TEST(c.next());
        BOOST_TEST(!c.done());
        // 3.
        BOOST_TEST(c.step_offset() == 2U);
        BOOST_TEST(c.step_size() == 1U);
        BOOST_TEST(!c.next());
        BOOST_TEST(c.done());
    }
    {
        fin::detail::Range<1> c{3};
        BOOST_TEST(c.steps() == 3U);
        // 1.
        BOOST_TEST(c.step_offset() == 0U);
        BOOST_TEST(c.step_size() == 1U);
        BOOST_TEST(c.next());
        BOOST_TEST(!c.done());
        // 2.
        BOOST_TEST(c.step_offset() == 1U);
        BOOST_TEST(c.step_size() == 1U);
        BOOST_TEST(c.next());
        BOOST_TEST(!c.done());
        // 3.
        BOOST_TEST(c.step_offset() == 2U);
        BOOST_TEST(c.step_size() == 1U);
        BOOST_TEST(!c.next());
        BOOST_TEST(c.done());
    }
    {
        fin::detail::Range<8> c{4};
        BOOST_TEST(c.steps() == 1U);
        // 0-3.
        BOOST_TEST(c.step_offset() == 0U);
        BOOST_TEST(c.step_size() == 4U);
        BOOST_TEST(!c.next());
        BOOST_TEST(c.done());
    }
    {
        fin::detail::Range<8> c{16};
        BOOST_TEST(c.steps() == 2U);
        // 0-7.
        BOOST_TEST(c.step_offset() == 0U);
        BOOST_TEST(c.step_size() == 8U);
        BOOST_TEST(c.next());
        BOOST_TEST(!c.done());
        // 8-15
        BOOST_TEST(c.step_offset() == 8U);
        BOOST_TEST(c.step_size() == 8U);
        BOOST_TEST(!c.next());
        BOOST_TEST(c.done());
    }
    {
        fin::detail::Range<8> c{20};
        BOOST_TEST(c.steps() == 3U);
        // 0-7.
        BOOST_TEST(c.step_offset() == 0U);
        BOOST_TEST(c.step_size() == 8U);
        BOOST_TEST(c.next());
        BOOST_TEST(!c.done());
        // 8-15
        BOOST_TEST(c.step_offset() == 8U);
        BOOST_TEST(c.step_size() == 8U);
        BOOST_TEST(c.next());
        BOOST_TEST(!c.done());
        // 16-19.
        BOOST_TEST(c.step_offset() == 16U);
        BOOST_TEST(c.step_size() == 4U);
        BOOST_TEST(!c.next());
        BOOST_TEST(c.done());
    }
}

BOOST_FIXTURE_TEST_CASE(MsgQueueCreateMarket, Fixture)
{
    mq.create_market(Now, MarketId, "EURUSD"sv, SettlDay, 0x1);

    Msg msg;
    BOOST_TEST(mq.pop(msg));
    BOOST_CHECK_EQUAL(msg.type, MsgType::CreateMarket);
    BOOST_CHECK_EQUAL(msg.time, ns_since_epoch(Now));
    const auto& body = msg.create_market;

    BOOST_CHECK_EQUAL(body.id, MarketId);
    BOOST_CHECK_EQUAL(strncmp(body.instr, "EURUSD", sizeof(body.instr)), 0);
    BOOST_CHECK_EQUAL(body.settl_day, SettlDay);
    BOOST_CHECK_EQUAL(body.state, 0x1U);
}

BOOST_FIXTURE_TEST_CASE(MsgQueueUpdateMarket, Fixture)
{
    mq.update_market(Now, MarketId, 0x1);

    Msg msg;
    BOOST_TEST(mq.pop(msg));
    BOOST_CHECK_EQUAL(msg.type, MsgType::UpdateMarket);
    BOOST_CHECK_EQUAL(msg.time, ns_since_epoch(Now));
    const auto& body = msg.update_market;

    BOOST_CHECK_EQUAL(body.id, MarketId);
    BOOST_CHECK_EQUAL(body.state, 0x1U);
}

BOOST_FIXTURE_TEST_CASE(MsgQueueCreateExec, Fixture)
{
    ConstExecPtr execs[2];
    execs[0] = make_intrusive<Exec>(Now, "MARAYL"sv, MarketId, 1_id64, 2_id64, "REF"sv, State::New,
                                    Side::Buy, 10_lts, 12345_tks, 10_lts, 0_lts, 0_cst, 0_lts,
                                    0_tks, 1_lts, 0_id64, 0_lts, 0_cst, LiqInd::None, Symbol{});
    execs[1] = make_intrusive<Exec>(Now + 1ms, "MARAYL"sv, MarketId, 3_id64, 2_id64, "REF"sv,
                                    State::Trade, Side::Buy, 10_lts, 12345_tks, 5_lts, 5_lts,
                                    61725_cst, 5_lts, 12345_tks, 1_lts, 4_id64, 0_lts, 0_cst,
                                    LiqInd::Maker, "GOSAYL"sv);
    mq.create_exec(execs);
    {
        Msg msg;
        BOOST_TEST(mq.pop(msg));
        BOOST_CHECK_EQUAL(msg.type, MsgType::CreateExec);
        BOOST_CHECK_EQUAL(msg.time, ns_since_epoch(Now));
        const auto& body = msg.create_exec;

        BOOST_CHECK_EQUAL(strncmp(body.accnt, "MARAYL", sizeof(body.accnt)), 0);
        BOOST_CHECK_EQUAL(body.market_id, MarketId);
        BOOST_CHECK_EQUAL(body.id, 1_id64);
        BOOST_CHECK_EQUAL(body.order_id, 2_id64);
        BOOST_CHECK_EQUAL(strncmp(body.ref, "REF", sizeof(body.ref)), 0);
        BOOST_CHECK_EQUAL(body.state, State::New);
        BOOST_CHECK_EQUAL(body.side, Side::Buy);
        BOOST_CHECK_EQUAL(body.lots, 10_lts);
        BOOST_CHECK_EQUAL(body.ticks, 12345_tks);
        BOOST_CHECK_EQUAL(body.resd_lots, 10_lts);
        BOOST_CHECK_EQUAL(body.exec_lots, 0_lts);
        BOOST_CHECK_EQUAL(body.exec_cost, 0_cst);
        BOOST_CHECK_EQUAL(body.last_lots, 0_lts);
        BOOST_CHECK_EQUAL(body.last_ticks, 0_tks);
        BOOST_CHECK_EQUAL(body.min_lots, 1_lts);
        BOOST_CHECK_EQUAL(body.match_id, 0_id64);
        BOOST_CHECK_EQUAL(body.posn_lots, 0_lts);
        BOOST_CHECK_EQUAL(body.posn_cost, 0_cst);
        BOOST_CHECK_EQUAL(body.liq_ind, LiqInd::None);
        BOOST_CHECK_EQUAL(strncmp(body.cpty, "", sizeof(body.cpty)), 0);
    }
    {
        Msg msg;
        BOOST_TEST(mq.pop(msg));
        BOOST_CHECK_EQUAL(msg.type, MsgType::CreateExec);
        BOOST_CHECK_EQUAL(msg.time, ns_since_epoch(Now + 1ms));
        const auto& body = msg.create_exec;

        BOOST_CHECK_EQUAL(body.market_id, MarketId);
        BOOST_CHECK_EQUAL(body.id, 3_id64);
        BOOST_CHECK_EQUAL(body.order_id, 2_id64);
        BOOST_CHECK_EQUAL(strncmp(body.accnt, "MARAYL", sizeof(body.accnt)), 0);
        BOOST_CHECK_EQUAL(strncmp(body.ref, "REF", sizeof(body.ref)), 0);
        BOOST_CHECK_EQUAL(body.state, State::Trade);
        BOOST_CHECK_EQUAL(body.side, Side::Buy);
        BOOST_CHECK_EQUAL(body.lots, 10_lts);
        BOOST_CHECK_EQUAL(body.ticks, 12345_tks);
        BOOST_CHECK_EQUAL(body.resd_lots, 5_lts);
        BOOST_CHECK_EQUAL(body.exec_lots, 5_lts);
        BOOST_CHECK_EQUAL(body.exec_cost, 61725_cst);
        BOOST_CHECK_EQUAL(body.last_lots, 5_lts);
        BOOST_CHECK_EQUAL(body.last_ticks, 12345_tks);
        BOOST_CHECK_EQUAL(body.min_lots, 1_lts);
        BOOST_CHECK_EQUAL(body.match_id, 4_id64);
        BOOST_CHECK_EQUAL(body.posn_lots, 0_lts);
        BOOST_CHECK_EQUAL(body.posn_cost, 0_cst);
        BOOST_CHECK_EQUAL(body.liq_ind, LiqInd::Maker);
        BOOST_CHECK_EQUAL(strncmp(body.cpty, "GOSAYL", sizeof(body.cpty)), 0);
    }
}

BOOST_FIXTURE_TEST_CASE(MsgQueueArchiveTrade, Fixture)
{
    vector<Id64> ids;
    ids.reserve(101);

    Id64 id{};
    generate_n(back_insert_iterator<decltype(ids)>(ids), ids.capacity(), [&id]() { return ++id; });
    mq.archive_trade(Now, MarketId, ids);

    auto it = ids.begin();
    while (it != ids.end()) {
        Msg msg;
        BOOST_TEST(mq.pop(msg));
        BOOST_CHECK_EQUAL(msg.type, MsgType::ArchiveTrade);
        BOOST_CHECK_EQUAL(msg.time, ns_since_epoch(Now));
        const auto& body = msg.archive_trade;
        BOOST_CHECK_EQUAL(body.market_id, MarketId);
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
