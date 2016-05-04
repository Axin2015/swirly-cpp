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
#include <swirly/ash/Config.hpp>

#include <swirly/tea/Test.hpp>

#include <map>

using namespace std;
using namespace swirly;

namespace {
template <typename FnT>
string applyCopy(FnT fn, string s)
{
  fn(s);
  return s;
}
} // anonymous

SWIRLY_TEST_CASE(ParseConfig)
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
  map<string, string> config;
  parseConfig(is, [&config](const auto& key, const auto& val) { config.emplace(key, val); });
  SWIRLY_CHECK(config.size() == 7);
  SWIRLY_CHECK(config["ab"] == "");
  SWIRLY_CHECK(config["cd"] == "");
  SWIRLY_CHECK(config["ef"] == "gh");
  SWIRLY_CHECK(config[""] == "ij");
  SWIRLY_CHECK(config["kl"] == "mn");
  SWIRLY_CHECK(config["op"] == "qr");
  SWIRLY_CHECK(config["st"] == "= uv =");
}

SWIRLY_TEST_CASE(VarInterp)
{
  auto fn = makeVarInterp([](const string& name) {
    string val;
    if (name == "FOO") {
      val = "101";
    } else if (name == "BAR") {
      val = "202";
    } else if (name == "BAZ") {
      val = "${FOO}${BAR}";
    }
    return val;
  });
  SWIRLY_CHECK(applyCopy(fn, "${FOO}") == "101");
  SWIRLY_CHECK(applyCopy(fn, "${BAR}") == "202");
  SWIRLY_CHECK(applyCopy(fn, "${BAZ}") == "101202");

  SWIRLY_CHECK(applyCopy(fn, " ${FOO} ") == " 101 ");
  // FIXME: recursion is not supported.
  SWIRLY_CHECK(applyCopy(fn, "${${BAR}}") == "}");
  SWIRLY_CHECK(applyCopy(fn, "${BAD}") == "");
}
