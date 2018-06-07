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
#include "Exception.hpp"

#include <iostream>

namespace swirly {
inline namespace fin {
using namespace std;

ServException::~ServException() = default;

void ServException::to_json(int status, const char* reason, const char* detail, ostream& os)
{
    os << "{\"status\":"sv << status     //
       << ",\"reason\":\""sv << reason   //
       << "\",\"detail\":\""sv << detail //
       << "\"}"sv;
}

BadRequestException::~BadRequestException() = default;

int BadRequestException::http_status() const noexcept
{
    return 400;
}

const char* BadRequestException::http_reason() const noexcept
{
    return "Bad Request";
}

AlreadyExistsException::~AlreadyExistsException() = default;

RefAlreadyExistsException::~RefAlreadyExistsException() = default;

InvalidException::~InvalidException() = default;

InvalidLotsException::~InvalidLotsException() = default;

InvalidTicksException::~InvalidTicksException() = default;

TooLateException::~TooLateException() = default;

ForbiddenException::~ForbiddenException() = default;

int ForbiddenException::http_status() const noexcept
{
    return 403;
}

const char* ForbiddenException::http_reason() const noexcept
{
    return "Forbidden";
}

InternalException::~InternalException() = default;

int InternalException::http_status() const noexcept
{
    return 500;
}

const char* InternalException::http_reason() const noexcept
{
    return "Internal Server Error";
}

MethodNotAllowedException::~MethodNotAllowedException() = default;

int MethodNotAllowedException::http_status() const noexcept
{
    return 405;
}

const char* MethodNotAllowedException::http_reason() const noexcept
{
    return "Method Not Allowed";
}

NotFoundException::~NotFoundException() = default;

int NotFoundException::http_status() const noexcept
{
    return 404;
}

const char* NotFoundException::http_reason() const noexcept
{
    return "Not Found";
}

MarketClosedException::~MarketClosedException() = default;

MarketNotFoundException::~MarketNotFoundException() = default;

OrderNotFoundException::~OrderNotFoundException() = default;

ServiceUnavailableException::~ServiceUnavailableException() = default;

int ServiceUnavailableException::http_status() const noexcept
{
    return 503;
}

const char* ServiceUnavailableException::http_reason() const noexcept
{
    return "Service Unavailable";
}

UnauthorizedException::~UnauthorizedException() = default;

int UnauthorizedException::http_status() const noexcept
{
    return 401;
}

const char* UnauthorizedException::http_reason() const noexcept
{
    return "Unauthorized";
}

} // namespace fin
} // namespace swirly
