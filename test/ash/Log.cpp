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

#include <swirly/ash/Finally.hpp>

#include <swirly/tea/Test.hpp>

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

void testLogger(int level, std::string_view msg)
{
  lastLevel = level;
  lastMsg.assign(msg.data(), msg.size());
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
  auto prevLevel = setLogLevel(LogInfo);
  auto prevLogger = setLogger(testLogger);
  auto finally = makeFinally([prevLevel, prevLogger]() {
    setLogLevel(prevLevel);
    setLogger(prevLogger);
  });

  SWIRLY_LOG(LogInfo, logMsg() << "test1: " << Foo<int, int>{10, 20});
  SWIRLY_CHECK(lastLevel == LogInfo);
  SWIRLY_CHECK(lastMsg == "test1: (10,20)");

  SWIRLY_CRIT(logMsg() << "test2: " << Foo<int, int>{10, 20});
  SWIRLY_CHECK(lastLevel == LogCrit);
  SWIRLY_CHECK(lastMsg == "test2: (10,20)");

  SWIRLY_ERROR(logMsg() << "test3: " << Foo<int, int>{10, 20});
  SWIRLY_CHECK(lastLevel == LogError);
  SWIRLY_CHECK(lastMsg == "test3: (10,20)");

  SWIRLY_WARNING(logMsg() << "test4: " << Foo<int, int>{10, 20});
  SWIRLY_CHECK(lastLevel == LogWarning);
  SWIRLY_CHECK(lastMsg == "test4: (10,20)");

  SWIRLY_NOTICE(logMsg() << "test5: " << Foo<int, int>{10, 20});
  SWIRLY_CHECK(lastLevel == LogNotice);
  SWIRLY_CHECK(lastMsg == "test5: (10,20)");

  SWIRLY_INFO(logMsg() << "test6: " << Foo<int, int>{10, 20});
  SWIRLY_CHECK(lastLevel == LogInfo);
  SWIRLY_CHECK(lastMsg == "test6: (10,20)");

  // This should not be logged.
  SWIRLY_DEBUG(logMsg() << "test7: " << Foo<int, int>{10, 20});
  SWIRLY_CHECK(lastLevel == LogInfo);
  SWIRLY_CHECK(lastMsg == "test6: (10,20)");
}
