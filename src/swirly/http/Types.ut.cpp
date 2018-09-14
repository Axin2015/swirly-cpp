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
#include "Types.hpp"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(TypesSuite)

BOOST_AUTO_TEST_CASE(TypesCase)
{
    BOOST_TEST(enum_string(HttpStatus::BadRequest) == "Bad Request");
    BOOST_TEST(enum_string(HttpStatus::Unauthorized) == "Unauthorized");
    BOOST_TEST(enum_string(HttpStatus::Forbidden) == "Forbidden");
    BOOST_TEST(enum_string(HttpStatus::NotFound) == "Not Found");
    BOOST_TEST(enum_string(HttpStatus::MethodNotAllowed) == "Method Not Allowed");
    BOOST_TEST(enum_string(HttpStatus::InternalServerError) == "Internal Server Error");
    BOOST_TEST(enum_string(HttpStatus::ServiceUnavailable) == "Service Unavailable");
}

BOOST_AUTO_TEST_SUITE_END()
