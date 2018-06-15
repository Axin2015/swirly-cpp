/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#ifndef SWIRLY_UTIL_EXCEPTION_HPP
#define SWIRLY_UTIL_EXCEPTION_HPP

#include <swirly/util/Limits.hpp>
#include <swirly/util/Stream.hpp>

#include <stdexcept>

namespace swirly {
inline namespace util {

using ErrMsg = StaticStream<MaxErrMsg>;

class SWIRLY_API Exception : public std::runtime_error {
  public:
    explicit Exception(std::error_code ec = std::error_code());
    Exception(int err, const std::error_category& ecat);
    Exception(std::error_code ec, std::string_view what);
    Exception(int err, const std::error_category& ecat, std::string_view what);
    ~Exception() override;

    // Copy.
    Exception(const Exception&) = default;
    Exception& operator=(const Exception&) = default;

    // Move.
    Exception(Exception&&) = default;
    Exception& operator=(Exception&&) = default;

    static void to_json(std::ostream& os, int status, const char* reason, const char* detail);

    const std::error_code& code() const noexcept { return ec_; }

  private:
    std::error_code ec_;
};

/**
 * Thread-local error message. This thread-local instance of StaticStream can be used to format
 * error messages before throwing. Note that the StaticStream is reset each time this function is
 * called.
 */
SWIRLY_API ErrMsg& err_msg() noexcept;

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_EXCEPTION_HPP
