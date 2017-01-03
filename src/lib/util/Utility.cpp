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
#include <swirly/util/Utility.hpp>

namespace swirly {

int hexDigits(int64_t i) noexcept
{
  int n{0};
  if (i & 0xffffffff00000000) {
    n += 8;
    i >>= 32;
  }
  if (i & 0xffff0000) {
    n += 4;
    i >>= 16;
  }
  if (i & 0xff00) {
    n += 2;
    i >>= 8;
  }
  if (i & 0xf0) {
    n += 2;
  } else if (i & 0x0f) {
    ++n;
  }
  return n;
}

} // swirly
