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
#include "MsgHandler.hpp"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {

struct MsgHandler : BasicMsgHandler<MsgHandler> {

    Time time{};
    int create_market_calls{0};
    int update_market_calls{0};
    int create_exec_calls{0};
    int archive_trade_calls{0};

    void on_create_market(Time time, const CreateMarket& body)
    {
        this->time = time;
        ++create_market_calls;
    }
    void on_update_market(Time time, const UpdateMarket& body)
    {
        this->time = time;
        ++update_market_calls;
    }
    void on_create_exec(Time time, const CreateExec& body)
    {
        this->time = time;
        ++create_exec_calls;
    }
    void on_archive_trade(Time time, const ArchiveTrade& body)
    {
        this->time = time;
        ++archive_trade_calls;
    }
};

} // namespace

BOOST_AUTO_TEST_SUITE(MsgHandlerSuite)

BOOST_AUTO_TEST_CASE(BasicMsgHandlerCase)
{
    MsgHandler h;
    Msg m;
    memset(&m, 0, sizeof(m));

    const auto now = UnixClock::now();
    m.type = MsgType::CreateMarket;
    m.time = ns_since_epoch(now + 1ms);
    BOOST_TEST(h.create_market_calls == 0);
    h.dispatch(m);
    BOOST_TEST(h.time == now + 1ms);
    BOOST_TEST(h.create_market_calls == 1);

    m.type = MsgType::UpdateMarket;
    m.time = ns_since_epoch(now + 2ms);
    BOOST_TEST(h.update_market_calls == 0);
    h.dispatch(m);
    BOOST_TEST(h.time == now + 2ms);
    BOOST_TEST(h.update_market_calls == 1);

    m.type = MsgType::CreateExec;
    m.time = ns_since_epoch(now + 3ms);
    BOOST_TEST(h.create_exec_calls == 0);
    h.dispatch(m);
    BOOST_TEST(h.time == now + 3ms);
    BOOST_TEST(h.create_exec_calls == 1);

    m.type = MsgType::ArchiveTrade;
    m.time = ns_since_epoch(now + 4ms);
    BOOST_TEST(h.archive_trade_calls == 0);
    h.dispatch(m);
    BOOST_TEST(h.time == now + 4ms);
    BOOST_TEST(h.archive_trade_calls == 1);
}

BOOST_AUTO_TEST_SUITE_END()
