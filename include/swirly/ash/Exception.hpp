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
#ifndef SWIRLY_ASH_EXCEPTION_HPP
#define SWIRLY_ASH_EXCEPTION_HPP

#include <swirly/ash/Defs.hpp>

#include <cstdarg> // va_list
#include <cstring> // strcpy()
#include <exception>

namespace swirly {

/**
 * @addtogroup Exception
 * @{
 */

class SWIRLY_API Exception : public std::exception {
 public:
  Exception() noexcept = default;

  ~Exception() noexcept override;

  // Copy.
  Exception(const Exception& rhs) noexcept { *this = rhs; }
  Exception& operator=(const Exception& rhs) noexcept
  {
    std::strcpy(msg_, rhs.msg_);
    return *this;
  }

  // Move.
  Exception(Exception&&) noexcept = default;
  Exception& operator=(Exception&&) noexcept = default;

  const char* what() const noexcept override;

  void format(const char* fmt, ...) noexcept;

  void format(const char* fmt, std::va_list args) noexcept;

 private:
  char msg_[128] = {'\0'};
};

template <typename ExceptionT>
ExceptionT makeException(const char* fmt, ...)
{
  ExceptionT e;
  va_list args;
  va_start(args, fmt);
  e.format(fmt, args);
  va_end(args);
  return e;
}

template <typename ExceptionT>
[[noreturn]] void throwException(const char* fmt, ...)
{
  ExceptionT e;
  va_list args;
  va_start(args, fmt);
  e.format(fmt, args);
  va_end(args);
  throw e;
}

/** @} */

} // swirly

#endif // SWIRLY_ASH_EXCEPTION_HPP
