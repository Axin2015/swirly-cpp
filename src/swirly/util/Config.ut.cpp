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
#include "Config.hpp"

#include <boost/test/unit_test.hpp>

#include <map>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(ConfigSuite)

BOOST_AUTO_TEST_CASE(ParseSectionSingleCase)
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
    parse_section(is, [&conf](const auto& key, const auto& val) { conf.emplace(key, val); });
    BOOST_TEST(conf.size() == 7U);
    BOOST_TEST(conf["ab"] == "");
    BOOST_TEST(conf["cd"] == "");
    BOOST_TEST(conf["ef"] == "gh");
    BOOST_TEST(conf[""] == "ij");
    BOOST_TEST(conf["kl"] == "mn");
    BOOST_TEST(conf["op"] == "qr");
    BOOST_TEST(conf["st"] == "= uv =");
    BOOST_TEST(is.eof());
}

BOOST_AUTO_TEST_CASE(ParseSectionMultiCase)
{
    const string text{R"(
# comment
  # indented comment
ab
cd=

[foo]
ef=gh
=ij

 [ bar ] 
kl = mn
 op = qr 
st = = uv =

)"};

    istringstream is{text};
    map<string, string> conf;

    string next;
    parse_section(is, [&conf](const auto& key, const auto& val) { conf.emplace(key, val); }, &next);
    BOOST_TEST(conf.size() == 2U);
    BOOST_TEST(conf["ab"] == "");
    BOOST_TEST(conf["cd"] == "");
    BOOST_TEST(next == "foo");
    BOOST_TEST(!is.fail());

    conf.clear();
    parse_section(is, [&conf](const auto& key, const auto& val) { conf.emplace(key, val); }, &next);
    BOOST_TEST(conf.size() == 2U);
    BOOST_TEST(conf["ef"] == "gh");
    BOOST_TEST(conf[""] == "ij");
    BOOST_TEST(next == "bar");
    BOOST_TEST(!is.fail());

    conf.clear();
    parse_section(is, [&conf](const auto& key, const auto& val) { conf.emplace(key, val); }, &next);
    BOOST_TEST(conf.size() == 3U);
    BOOST_TEST(conf["kl"] == "mn");
    BOOST_TEST(conf["op"] == "qr");
    BOOST_TEST(conf["st"] == "= uv =");
    BOOST_TEST(next.empty());
    BOOST_TEST(is.eof());
}

BOOST_AUTO_TEST_CASE(ConfigOverrideCase)
{
    const string text{R"(
foo=101
bar=202

[session]

bar=303
baz=404
)"};

    istringstream is{text};
    string next;

    Config parent;
    parent.read_section(is, next);

    BOOST_TEST(parent.size() == 2U);
    BOOST_TEST(parent.get<int>("foo", 0) == 101);
    BOOST_TEST(parent.get<int>("bar", 0) == 202);
    BOOST_TEST(next == "session");
    BOOST_TEST(!is.fail());

    Config child;
    child.read_section(is, next);
    child.set_parent(parent);

    BOOST_TEST(child.size() == 2U);
    BOOST_TEST(child.get<int>("foo", 0) == 101);
    BOOST_TEST(child.get<int>("bar", 0) == 303);
    BOOST_TEST(child.get<int>("baz", 0) == 404);
    BOOST_TEST(next.empty());
    BOOST_TEST(is.eof());
}

BOOST_AUTO_TEST_SUITE_END()
