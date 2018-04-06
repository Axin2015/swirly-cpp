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
#include "Instr.hpp"

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(InstrSuite)

BOOST_AUTO_TEST_CASE(InstrToStringCase)
{
    Instr instr{1_id32, "EURUSD"sv, "EURUSD"sv, "EUR"sv, "USD"sv, "SWIRLY"sv, 1000000,
                1,      1,          10000,      4,       1_lts,   10_lts};

    BOOST_TEST(toString(instr) == //
               "{\"symbol\":\"EURUSD\""
               ",\"display\":\"EURUSD\""
               ",\"baseAsset\":\"EUR\""
               ",\"termCcy\":\"USD\""
               ",\"broker\":\"SWIRLY\""
               ",\"lotNumer\":1000000"
               ",\"lotDenom\":1"
               ",\"tickNumer\":1"
               ",\"tickDenom\":10000"
               ",\"pipDp\":4"
               ",\"minLots\":1"
               ",\"maxLots\":10"
               "}");
}

BOOST_AUTO_TEST_SUITE_END()
