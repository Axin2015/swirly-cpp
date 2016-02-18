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
#ifndef SWIRLY_FIR_EXCEPTION_HPP
#define SWIRLY_FIR_EXCEPTION_HPP

#include <swirly/ash/Exception.hpp>

namespace swirly {

/**
 * @addtogroup Exception
 * @{
 */

class SWIRLY_API ParseException : public Exception {
 public:
  explicit ParseException(const std::string_view& what) noexcept : Exception{what} {}
  ~ParseException() noexcept override;

  // Copy.
  ParseException(const ParseException&) noexcept = default;
  ParseException& operator=(const ParseException&) noexcept = default;

  // Move.
  ParseException(ParseException&&) noexcept = default;
  ParseException& operator=(ParseException&&) noexcept = default;
};

/** @} */

} // swirly

#endif // SWIRLY_FIR_EXCEPTION_HPP
