/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#include <swirly/ash/Log.hpp>

#include <boost/test/unit_test.hpp>

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
} // anonymous

BOOST_AUTO_TEST_SUITE(LogSuite)

BOOST_AUTO_TEST_CASE(LogLabelCase)
{
  BOOST_CHECK_EQUAL(strcmp(logLabel(-1), "CRIT"), 0);
  BOOST_CHECK_EQUAL(strcmp(logLabel(LogCrit), "CRIT"), 0);
  BOOST_CHECK_EQUAL(strcmp(logLabel(LogError), "ERROR"), 0);
  BOOST_CHECK_EQUAL(strcmp(logLabel(LogWarning), "WARNING"), 0);
  BOOST_CHECK_EQUAL(strcmp(logLabel(LogNotice), "NOTICE"), 0);
  BOOST_CHECK_EQUAL(strcmp(logLabel(LogInfo), "INFO"), 0);
  BOOST_CHECK_EQUAL(strcmp(logLabel(LogDebug), "DEBUG"), 0);
  BOOST_CHECK_EQUAL(strcmp(logLabel(99), "DEBUG"), 0);
}

BOOST_AUTO_TEST_CASE(LogMacroCase)
{
  SWIRLY_LOG(LogInfo, logMsg() << "test: " << Foo<int, int>{10, 20});
  SWIRLY_CRIT(logMsg() << "test: " << Foo<int, int>{10, 20});
  SWIRLY_ERROR(logMsg() << "test" << Foo<int, int>{10, 20});
  SWIRLY_WARNING(logMsg() << "test" << Foo<int, int>{10, 20});
  SWIRLY_NOTICE(logMsg() << "test" << Foo<int, int>{10, 20});
  SWIRLY_INFO(logMsg() << "test" << Foo<int, int>{10, 20});
  SWIRLY_DEBUG(logMsg() << "test" << Foo<int, int>{10, 20});
}

BOOST_AUTO_TEST_SUITE_END()
