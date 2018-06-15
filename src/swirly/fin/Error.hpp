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
#ifndef SWIRLY_FIN_ERROR_HPP
#define SWIRLY_FIN_ERROR_HPP

#include <swirly/Config.h>

#include <system_error>

namespace swirly {
inline namespace fin {

enum class Error : int {
    BadRequest = 40000,
    AlreadyExists = 40001,
    RefAlreadyExists = 40002,
    Invalid = 40003,
    InvalidLots = 40004,
    InvalidTicks = 40005,
    ProtocolError = 40006,
    TooLate = 40007,
    Unauthorized = 40100,
    Forbidden = 40300,
    NotFound = 40400,
    AccntNotFound = 40401,
    MarketNotFound = 40402,
    OrderNotFound = 40403,
    MethodNotAllowed = 40500,
    InternalError = 50000,
    DatabaseError = 50001,
    ServiceUnavailable = 50300,
    MarketClosed = 50301
};

SWIRLY_API const std::error_category& error_category() noexcept;
SWIRLY_API std::error_code make_error_code(Error err);

} // namespace fin
} // namespace swirly

namespace std {
template <>
struct is_error_code_enum<swirly::fin::Error> : true_type {
};
} // namespace std

#endif // SWIRLY_FIN_ERROR_HPP
