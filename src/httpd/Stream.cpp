/*
 * Swirly Order-Book and Matching-Engine.
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
#include "Stream.hpp"

namespace swirly {
namespace mg {

StreamBuf::~StreamBuf() noexcept
{
  mbuf_free(&buf_);
}

StreamBuf::int_type StreamBuf::overflow(int_type c) noexcept
{
  if (c != traits_type::eof()) {
    const char z = c;
    if (mbuf_append(&buf_, &z, 1) != 1)
      c = traits_type::eof();
  }
  return c;
}

std::streamsize StreamBuf::xsputn(const char_type* s, std::streamsize count) noexcept
{
  return mbuf_append(&buf_, s, count);
}

OStream::~OStream() noexcept = default;

} // mg
} // swirly
