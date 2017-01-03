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
#include <swirly/util/VarSub.hpp>

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

namespace {

template <typename FnT>
string applyCopy(FnT fn, string s)
{
  fn(s);
  return s;
}

string getVar(const string& name)
{
  string val;
  if (name == "FOO") {
    val = "101";
  } else if (name == "BAR") {
    val = "202";
  } else if (name == "BAZ") {
    val = "FOO";
  } else if (name == "QUX") {
    val = "BAR";
  } else if (name == "FOOBAR") {
    val = "${FOO}${BAR}";
  }
  return val;
}

} // anonymous

SWIRLY_TEST_CASE(VarSubBasic)
{
  VarSub fn{getVar};

  SWIRLY_CHECK(applyCopy(fn, "${FOO}") == "101");
  SWIRLY_CHECK(applyCopy(fn, "${BAR}") == "202");
  SWIRLY_CHECK(applyCopy(fn, "<${FOO}>") == "<101>");
  SWIRLY_CHECK(applyCopy(fn, "<${FOO} ${BAR}>") == "<101 202>");
}

SWIRLY_TEST_CASE(VarSubEmpty)
{
  VarSub fn{getVar};

  SWIRLY_CHECK(applyCopy(fn, "${}").empty());
  SWIRLY_CHECK(applyCopy(fn, "${123}").empty());
  SWIRLY_CHECK(applyCopy(fn, "${EMPTY}").empty());
}

SWIRLY_TEST_CASE(VarSubEscape)
{
  VarSub fn{getVar};

  SWIRLY_CHECK(applyCopy(fn, "\\\\") == "\\");
  SWIRLY_CHECK(applyCopy(fn, "\\\\>") == "\\>");
  SWIRLY_CHECK(applyCopy(fn, "<\\\\") == "<\\");
  SWIRLY_CHECK(applyCopy(fn, "\\${FOO}") == "${FOO}");
  SWIRLY_CHECK(applyCopy(fn, "$\\{FOO}") == "${FOO}");
  SWIRLY_CHECK(applyCopy(fn, "${\\FOO}") == "101");
  SWIRLY_CHECK(applyCopy(fn, "${FOO\\}") == "${FOO}");
  SWIRLY_CHECK(applyCopy(fn, "${FOO}\\") == "101\\");
}

SWIRLY_TEST_CASE(VarSubPartial)
{
  VarSub fn{getVar};

  SWIRLY_CHECK(applyCopy(fn, "$") == "$");
  SWIRLY_CHECK(applyCopy(fn, "{") == "{");
  SWIRLY_CHECK(applyCopy(fn, "}") == "}");
  SWIRLY_CHECK(applyCopy(fn, "$FOO") == "$FOO");
  SWIRLY_CHECK(applyCopy(fn, "{FOO") == "{FOO");
  SWIRLY_CHECK(applyCopy(fn, "${FOO") == "${FOO");
  SWIRLY_CHECK(applyCopy(fn, "FOO}") == "FOO}");
  SWIRLY_CHECK(applyCopy(fn, "$${FOO}") == "$101");
}

SWIRLY_TEST_CASE(VarSubNested)
{
  VarSub fn{getVar};

  SWIRLY_CHECK(applyCopy(fn, "${FOOBAR}") == "101202");
  SWIRLY_CHECK(applyCopy(fn, "${${BAZ}}") == "101");
  SWIRLY_CHECK(applyCopy(fn, "${${BAZ}BAR}") == "101202");
  SWIRLY_CHECK(applyCopy(fn, "${FOO${QUX}}") == "101202");
  SWIRLY_CHECK(applyCopy(fn, "${${BAZ}${QUX}}") == "101202");
}

SWIRLY_TEST_CASE(VarSubLoop)
{
  VarSub fn{[](const string& name) {
    string val;
    if (name == "FOO") {
      val = "${BAR}";
    } else if (name == "BAR") {
      val = "${BAZ}";
    } else if (name == "BAZ") {
      val = "${QUX}";
    } else if (name == "QUX") {
      val = "${FOO}";
    }
    return val;
  }};

  SWIRLY_CHECK(applyCopy(fn, "${FOO}").empty());
  SWIRLY_CHECK(applyCopy(fn, "${FOO}${FOO}").empty());
  SWIRLY_CHECK(applyCopy(fn, "${FOO${FOO}}").empty());
  SWIRLY_CHECK(applyCopy(fn, "<${FOO}>") == "<>");
  SWIRLY_CHECK(applyCopy(fn, "<${FOO${FOO}}>") == "<>");
  SWIRLY_CHECK(applyCopy(fn, "<${FOO} ${FOO}>") == "< >");
}
