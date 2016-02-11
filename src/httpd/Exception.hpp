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
#ifndef SWIRLY_HTTPD_EXCEPTION_HPP
#define SWIRLY_HTTPD_EXCEPTION_HPP

#include <swirly/ash/Exception.hpp>

namespace swirly {
namespace mg {

/**
 * @addtogroup App
 * @{
 */

class Error : public Exception {
 public:
  Error() noexcept = default;

  ~Error() noexcept;

  // Copy.
  Error(const Error&) noexcept = default;
  Error& operator=(const Error&) noexcept = default;

  // Move.
  Error(Error&&) noexcept = default;
  Error& operator=(Error&&) noexcept = default;
};

/** @} */

} // mg
} // swirly

#endif // SWIRLY_HTTPD_EXCEPTION_HPP
