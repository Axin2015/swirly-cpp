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

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(InstrSuite)

BOOST_AUTO_TEST_CASE(InstrToStringCase)
{
    Instr instr{1_id32, "EURUSD"sv, "EURUSD"sv, "EUR"sv, "USD"sv, 1000000,
                1,      1,          10000,      4,       1_lts,   10_lts};

    BOOST_TEST(to_string(instr) == //
               "{\"symbol\":\"EURUSD\""
               ",\"display\":\"EURUSD\""
               ",\"base_asset\":\"EUR\""
               ",\"term_ccy\":\"USD\""
               ",\"lot_numer\":1000000"
               ",\"lot_denom\":1"
               ",\"tick_numer\":1"
               ",\"tick_denom\":10000"
               ",\"pip_dp\":4"
               ",\"min_lots\":1"
               ",\"max_lots\":10"
               "}");
}

BOOST_AUTO_TEST_SUITE_END()
