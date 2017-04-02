/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#include <swirly/util/Conf.hpp>

#include <swirly/unit/Test.hpp>

#include <map>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(ParsePairs)
{
    const string text{R"(
# comment
  # indented comment
ab
cd=
ef=gh
=ij

kl = mn
 op = qr 
st = = uv =

)"};

    istringstream is{text};
    map<string, string> conf;
    parsePairs(is, [&conf](const auto& key, const auto& val) { conf.emplace(key, val); });
    SWIRLY_CHECK(conf.size() == 7);
    SWIRLY_CHECK(conf["ab"] == "");
    SWIRLY_CHECK(conf["cd"] == "");
    SWIRLY_CHECK(conf["ef"] == "gh");
    SWIRLY_CHECK(conf[""] == "ij");
    SWIRLY_CHECK(conf["kl"] == "mn");
    SWIRLY_CHECK(conf["op"] == "qr");
    SWIRLY_CHECK(conf["st"] == "= uv =");
}
