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

#include <swirly/tea/Test.hpp>

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

SWIRLY_TEST_CASE(LogLabel)
{
  SWIRLY_CHECK(strcmp(logLabel(-1), "CRIT") == 0);
  SWIRLY_CHECK(strcmp(logLabel(LogCrit), "CRIT") == 0);
  SWIRLY_CHECK(strcmp(logLabel(LogError), "ERROR") == 0);
  SWIRLY_CHECK(strcmp(logLabel(LogWarning), "WARNING") == 0);
  SWIRLY_CHECK(strcmp(logLabel(LogNotice), "NOTICE") == 0);
  SWIRLY_CHECK(strcmp(logLabel(LogInfo), "INFO") == 0);
  SWIRLY_CHECK(strcmp(logLabel(LogDebug), "DEBUG") == 0);
  SWIRLY_CHECK(strcmp(logLabel(99), "DEBUG") == 0);
}

SWIRLY_TEST_CASE(LogMacro)
{
  SWIRLY_LOG(LogInfo, logMsg() << "test: " << Foo<int, int>{10, 20});
  SWIRLY_CRIT(logMsg() << "test: " << Foo<int, int>{10, 20});
  SWIRLY_ERROR(logMsg() << "test" << Foo<int, int>{10, 20});
  SWIRLY_WARNING(logMsg() << "test" << Foo<int, int>{10, 20});
  SWIRLY_NOTICE(logMsg() << "test" << Foo<int, int>{10, 20});
  SWIRLY_INFO(logMsg() << "test" << Foo<int, int>{10, 20});
  SWIRLY_DEBUG(logMsg() << "test" << Foo<int, int>{10, 20});
}
