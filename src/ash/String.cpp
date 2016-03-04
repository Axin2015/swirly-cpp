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
#include <swirly/ash/String.hpp>

using namespace std;

namespace swirly {

unsigned long stoul(string_view sv) noexcept
{
  long l{0};
  auto it = sv.begin(), end = sv.end();

  // Skip leading whitespace.
  while (it != end && isspace(*it)) {
    ++it;
  }
  while (it != end && isdigit(*it)) {
    l *= 10;
    l += *it - '0';
    ++it;
  }
  return l;
}

} // swirly
