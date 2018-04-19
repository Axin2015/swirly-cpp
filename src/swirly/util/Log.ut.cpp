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
#include "Log.hpp"

#include <swirly/util/Finally.hpp>

#include <boost/test/unit_test.hpp>

#include <cstring>

using namespace std;
using namespace swirly;

namespace {

template <typename T, typename U>
struct Foo {
    T first;
    U second;
};

ostream& operator<<(ostream& os, const Foo<int, int>& val)
{
    return os << '(' << val.first << ',' << val.second << ')';
}

int lastLevel{};
string lastMsg{};

void testLogger(int level, string_view msg)
{
    lastLevel = level;
    lastMsg.assign(msg.data(), msg.size());
}

} // namespace

BOOST_AUTO_TEST_SUITE(LogSuite)

BOOST_AUTO_TEST_CASE(LogLabelCase)
{
    BOOST_TEST(strcmp(logLabel(-1), "CRIT") == 0);
    BOOST_TEST(strcmp(logLabel(Log::Crit), "CRIT") == 0);
    BOOST_TEST(strcmp(logLabel(Log::Error), "ERROR") == 0);
    BOOST_TEST(strcmp(logLabel(Log::Warning), "WARNING") == 0);
    BOOST_TEST(strcmp(logLabel(Log::Notice), "NOTICE") == 0);
    BOOST_TEST(strcmp(logLabel(Log::Info), "INFO") == 0);
    BOOST_TEST(strcmp(logLabel(Log::Debug), "DEBUG") == 0);
    BOOST_TEST(strcmp(logLabel(99), "DEBUG") == 0);
}

BOOST_AUTO_TEST_CASE(LogMacroCase)
{
    auto prevLevel = setLogLevel(Log::Info);
    auto prevLogger = setLogger(testLogger);
    // clang-format off
    const auto finally = makeFinally([prevLevel, prevLogger]() noexcept {
        setLogLevel(prevLevel);
        setLogger(prevLogger);
    });
    // clang-format on

    SWIRLY_LOG(Log::Info) << "test1: "sv << Foo<int, int>{10, 20};
    BOOST_TEST(lastLevel == Log::Info);
    BOOST_TEST(lastMsg == "test1: (10,20)");

    SWIRLY_CRIT << "test2: "sv << Foo<int, int>{10, 20};
    BOOST_TEST(lastLevel == Log::Crit);
    BOOST_TEST(lastMsg == "test2: (10,20)");

    SWIRLY_ERROR << "test3: "sv << Foo<int, int>{10, 20};
    BOOST_TEST(lastLevel == Log::Error);
    BOOST_TEST(lastMsg == "test3: (10,20)");

    SWIRLY_WARNING << "test4: "sv << Foo<int, int>{10, 20};
    BOOST_TEST(lastLevel == Log::Warning);
    BOOST_TEST(lastMsg == "test4: (10,20)");

    SWIRLY_NOTICE << "test5: "sv << Foo<int, int>{10, 20};
    BOOST_TEST(lastLevel == Log::Notice);
    BOOST_TEST(lastMsg == "test5: (10,20)");

    SWIRLY_INFO << "test6: "sv << Foo<int, int>{10, 20};
    BOOST_TEST(lastLevel == Log::Info);
    BOOST_TEST(lastMsg == "test6: (10,20)");

    // This should not be logged.
    SWIRLY_DEBUG << "test7: "sv << Foo<int, int>{10, 20};
    BOOST_TEST(lastLevel == Log::Info);
    BOOST_TEST(lastMsg == "test6: (10,20)");
}

BOOST_AUTO_TEST_SUITE_END()
