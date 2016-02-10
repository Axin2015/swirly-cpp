/*
 * Swirly Order-Book and Matching-Engine.
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
#include <swirly/ash/Exception.hpp>

#include <boost/test/unit_test.hpp>

using namespace swirly;

BOOST_AUTO_TEST_SUITE(ExceptionSuite)

BOOST_AUTO_TEST_CASE(FormatCase)
{
  using namespace std::literals::string_literals;

  Exception e;

  e.format("[%d]", 123);
  BOOST_CHECK(std::strcmp(e.what(), "[123]") == 0);

  e.format("[%.*s]", 3, "Foox");
  BOOST_CHECK(std::strcmp(e.what(), "[Foo]") == 0);

  e.format("[%.*s]", SWIRLY_STR("Bar"s));
  BOOST_CHECK(std::strcmp(e.what(), "[Bar]") == 0);
}

BOOST_AUTO_TEST_CASE(MakeExceptionCase)
{
  auto e = makeException<Exception>("[%d]", 123);
  BOOST_CHECK(std::strcmp(e.what(), "[123]") == 0);
}

BOOST_AUTO_TEST_CASE(ThrowExceptionCase)
{
  BOOST_CHECK_THROW(throwException<Exception>("[%d]", 123), Exception);
}

BOOST_AUTO_TEST_SUITE_END()
