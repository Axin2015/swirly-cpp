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
#include "Types.hpp"

#include <swirly/app/Error.hpp>

namespace swirly {
inline namespace http {

const char* enum_string(HttpStatus status) noexcept
{
    switch (static_cast<int>(status)) {
#define XX(num, name, string)                                                                      \
    case num:                                                                                      \
        return #string;
        HTTP_STATUS_MAP(XX)
#undef XX
    }
    std::terminate();
}

HttpStatus http_status(const std::error_code& ec)
{
    if (ec.category() != error_category()) {
        return HttpStatus::InternalServerError;
    }
    HttpStatus status;
    switch (static_cast<Error>(ec.value())) {
    case Error::BadRequest:
    case Error::AlreadyExists:
    case Error::RefAlreadyExists:
    case Error::Invalid:
    case Error::InvalidLots:
    case Error::InvalidTicks:
    case Error::ProtocolError:
    case Error::TooLate:
        status = HttpStatus::BadRequest;
        break;
    case Error::Unauthorized:
        status = HttpStatus::Unauthorized;
        break;
    case Error::Forbidden:
        status = HttpStatus::Forbidden;
        break;
    case Error::NotFound:
    case Error::AccntNotFound:
    case Error::MarketNotFound:
    case Error::OrderNotFound:
        status = HttpStatus::NotFound;
        break;
    case Error::MethodNotAllowed:
        status = HttpStatus::MethodNotAllowed;
        break;
    case Error::InternalError:
    case Error::DatabaseError:
        status = HttpStatus::InternalServerError;
        break;
    case Error::ServiceUnavailable:
    case Error::MarketClosed:
        status = HttpStatus::ServiceUnavailable;
        break;
    default:
        status = HttpStatus::InternalServerError;
        break;
    }
    return status;
}

} // namespace http
} // namespace swirly
