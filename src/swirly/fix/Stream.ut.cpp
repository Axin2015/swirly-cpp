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
#include "Stream.hpp"

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

BOOST_AUTO_TEST_SUITE(StreamSuite)

BOOST_AUTO_TEST_CASE(StreamEmptyCase)
{
    Buffer buf{16};
    {
        FixStream os{buf};
        BOOST_TEST(buf.empty());
    }
    BOOST_TEST(buf.empty());
    {
        FixStream os{buf};
        os.commit();
        BOOST_TEST(buf.empty());
    }
    BOOST_TEST(buf.empty());
}

BOOST_AUTO_TEST_CASE(StreamNoBodyCase)
{
    Buffer buf{16};
    {
        FixStream os{buf};
        os.reset({4, 3});
        os.commit();
        BOOST_TEST(buf
                   == "8=FIX.4.3\1"
                      "9=0000\1"
                      "10=087\1"sv);
    }
    BOOST_TEST(buf
               == "8=FIX.4.3\1"
                  "9=0000\1"
                  "10=087\1"sv);
}

BOOST_AUTO_TEST_CASE(StreamMsgTypeCase)
{
    Buffer buf{16};
    {
        FixStream os{buf};
        os.reset({4, 3});
        os << "35=D\1";
        os.commit();
        BOOST_TEST(buf
                   == "8=FIX.4.3\1"
                      "9=0005\1"
                      "35=D\1"
                      "10=070\1"sv);
    }
    BOOST_TEST(buf
               == "8=FIX.4.3\1"
                  "9=0005\1"
                  "35=D\1"
                  "10=070\1"sv);
}

BOOST_AUTO_TEST_CASE(StreamNoCommitCase)
{
    Buffer buf{16};
    {
        FixStream os{buf};
        os.reset({4, 3});
        os << "35=D\1";
        BOOST_TEST(buf.empty());
    }
    BOOST_TEST(buf.empty());
}

BOOST_AUTO_TEST_CASE(StreamMultiCase)
{
    Buffer buf{16};
    {
        FixStream os{buf};
        os.reset({4, 2});
        os << "35=A\1";
        os.commit();
        BOOST_TEST(buf
                   == "8=FIX.4.2\1"
                      "9=0005\1"
                      "35=A\1"
                      "10=066\1"sv);
    }
    BOOST_TEST(buf
               == "8=FIX.4.2\1"
                  "9=0005\1"
                  "35=A\1"
                  "10=066\1"sv);
    {
        FixStream os{buf};
        os.reset({4, 3});
        os << "35=D\1";
        os.commit();
        BOOST_TEST(buf
                   == "8=FIX.4.2\1"
                      "9=0005\1"
                      "35=A\1"
                      "10=066\1"
                      "8=FIX.4.3\1"
                      "9=0005\1"
                      "35=D\1"
                      "10=070\1"sv);
    }
    BOOST_TEST(buf
               == "8=FIX.4.2\1"
                  "9=0005\1"
                  "35=A\1"
                  "10=066\1"
                  "8=FIX.4.3\1"
                  "9=0005\1"
                  "35=D\1"
                  "10=070\1"sv);
}

BOOST_AUTO_TEST_SUITE_END()
