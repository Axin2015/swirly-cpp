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

    int createMarketCalls{0};
    int updateMarketCalls{0};
    int createExecCalls{0};
    int archiveTradeCalls{0};

    void onCreateMarket(const CreateMarket& body) { ++createMarketCalls; }
    void onUpdateMarket(const UpdateMarket& body) { ++updateMarketCalls; }
    void onCreateExec(const CreateExec& body) { ++createExecCalls; }
    void onArchiveTrade(const ArchiveTrade& body) { ++archiveTradeCalls; }
};

} // namespace

BOOST_AUTO_TEST_SUITE(MsgHandlerSuite)

BOOST_AUTO_TEST_CASE(BasicMsgHandlerCase)
{
    MsgHandler h;
    Msg m;
    memset(&m, 0, sizeof(m));

    m.type = MsgType::CreateMarket;
    BOOST_TEST(h.createMarketCalls == 0);
    h.dispatch(m);
    BOOST_TEST(h.createMarketCalls == 1);

    m.type = MsgType::UpdateMarket;
    BOOST_TEST(h.updateMarketCalls == 0);
    h.dispatch(m);
    BOOST_TEST(h.updateMarketCalls == 1);

    m.type = MsgType::CreateExec;
    BOOST_TEST(h.createExecCalls == 0);
    h.dispatch(m);
    BOOST_TEST(h.createExecCalls == 1);

    m.type = MsgType::ArchiveTrade;
    BOOST_TEST(h.archiveTradeCalls == 0);
    h.dispatch(m);
    BOOST_TEST(h.archiveTradeCalls == 1);
}

BOOST_AUTO_TEST_SUITE_END()
