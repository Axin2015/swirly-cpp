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
#include <swirly/ws/Page.hpp>

#include <swirly/util/Tokeniser.hpp>

#include <tuple>

using namespace std;

namespace swirly {

Page parseQuery(string_view query) noexcept
{
  Page page;
  Tokeniser toks{query, "&;"_sv};
  while (!toks.empty()) {
    string_view key, val;
    tie(key, val) = splitPair(toks.top(), '=');
    if (key == "offset"_sv) {
      page.offset = stou64(val);
    } else if (key == "limit"_sv) {
      page.limit = stou64(val);
    }
    toks.pop();
  }
  return page;
}

} // swirly
