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
#include "Header.hpp"

#include <swirly/util/Date.hpp>

#include <boost/test/unit_test.hpp>

namespace swirly {
inline namespace sys {
bool operator==(const Buffer& buf, std::string_view sv);
bool operator!=(const Buffer& buf, std::string_view sv);
bool operator==(std::string_view sv, const Buffer& buf);
bool operator!=(std::string_view sv, const Buffer& buf);
std::ostream& operator<<(std::ostream& os, const Buffer& buf);
} // namespace sys
} // namespace swirly

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(HeaderSuite)

BOOST_AUTO_TEST_CASE(HeaderDirectCase)
{
    Buffer buf{16};
    FixStream os{buf};
    os.reset({4, 3});
    os << "35=h\1"
          "49=CLIENT\1"
          "56=SERVER\1"
          "34=1\1"
          "52=20180824-05:32:29.123\1"
          "336=OPEN\1"
          "340=2\1";

    os.commit();
    BOOST_TEST(buf
               == "8=FIX.4.3\1"
                  "9=0070\1"
                  "35=h\1"
                  "49=CLIENT\1"
                  "56=SERVER\1"
                  "34=1\1"
                  "52=20180824-05:32:29.123\1"
                  "336=OPEN\1"
                  "340=2\1"
                  "10=023\1"sv);
}

BOOST_AUTO_TEST_CASE(HeaderFieldCase)
{
    FixHeaderView hdr;
    get<Tag::MsgType>(hdr) = "h"sv;
    get<Tag::SenderCompId>(hdr) = "CLIENT"sv;
    get<Tag::TargetCompId>(hdr) = "SERVER"sv;
    get<Tag::MsgSeqNum>(hdr) = 1;
    get<Tag::SendingTime>(hdr) = parse_time("20180824-05:32:29.123"sv).value;

    Buffer buf{16};
    FixStream os{buf};
    os.reset({4, 3});
    os << hdr << put_fix<Tag::TradingSessionId>("OPEN"sv) << put_fix<Tag::TradSesStatus>(2);
    os.commit();
    BOOST_TEST(buf
               == "8=FIX.4.3\1"
                  "9=0070\1"
                  "35=h\1"
                  "49=CLIENT\1"
                  "56=SERVER\1"
                  "34=1\1"
                  "52=20180824-05:32:29.123\1"
                  "336=OPEN\1"
                  "340=2\1"
                  "10=023\1"sv);
}

BOOST_AUTO_TEST_SUITE_END()
