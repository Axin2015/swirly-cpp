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
#ifndef SWIRLY_ASH_STREAM_HPP
#define SWIRLY_ASH_STREAM_HPP

#include <swirly/ash/Defs.hpp>

#include <ostream>

namespace swirly {

/**
 * @addtogroup Util
 * @{
 */

/**
 * Stream joiner. This is a simplified version of std::experimental::ostream_joiner, intended as a
 * placeholder until TS v2 is more widely available.
 */
class SWIRLY_API OStreamJoiner {
 public:
  using char_type = char;
  using traits_type = std::char_traits<char_type>;
  using ostream_type = std::ostream;
  using value_type = void;
  using difference_type = void;
  using pointer = void;
  using reference = void;
  using iterator_category = std::output_iterator_tag;

  OStreamJoiner(std::ostream& os, const char delim) noexcept : os_{&os}, delim_{delim} {}
  ~OStreamJoiner() noexcept;

  // Copy.
  OStreamJoiner(const OStreamJoiner&) = default;
  OStreamJoiner& operator=(const OStreamJoiner&) = default;

  // Move.
  OStreamJoiner(OStreamJoiner&&) = default;
  OStreamJoiner& operator=(OStreamJoiner&&) = default;

  template <typename ValueT>
  OStreamJoiner& operator=(const ValueT& value)
  {
    if (!first_)
      *os_ << delim_;
    first_ = false;
    *os_ << value;
    return *this;
  }
  OStreamJoiner& operator*() noexcept { return *this; }
  OStreamJoiner& operator++() noexcept { return *this; }
  OStreamJoiner& operator++(int)noexcept { return *this; }

 private:
  std::ostream* os_;
  char delim_;
  bool first_{true};
};

/** @} */

} // swirly

#endif // SWIRLY_ASH_STREAM_HPP
