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
#include <swirly/elm/Exception.hpp>

using namespace std;

namespace swirly {

ServException::~ServException() noexcept = default;

BadRequestException::~BadRequestException() noexcept = default;

AlreadyExistsException::~AlreadyExistsException() noexcept = default;

RefAlreadyExistsException::~RefAlreadyExistsException() noexcept = default;

InvalidException::~InvalidException() noexcept = default;

InvalidLotsException::~InvalidLotsException() noexcept = default;

InvalidTicksException::~InvalidTicksException() noexcept = default;

TooLateException::~TooLateException() noexcept = default;

ForbiddenException::~ForbiddenException() noexcept = default;

InternalException::~InternalException() noexcept = default;

LiquidityUnavailableException::~LiquidityUnavailableException() noexcept = default;

MethodNotAllowedException::~MethodNotAllowedException() noexcept = default;

NotFoundException::~NotFoundException() noexcept = default;

MarketClosedException::~MarketClosedException() noexcept = default;

MarketNotFoundException::~MarketNotFoundException() noexcept = default;

OrderNotFoundException::~OrderNotFoundException() noexcept = default;

QuoteNotFoundException::~QuoteNotFoundException() noexcept = default;

TraderNotFoundException::~TraderNotFoundException() noexcept = default;

ServiceUnavailableException::~ServiceUnavailableException() noexcept = default;

UnauthorizedException::~UnauthorizedException() noexcept = default;

} // swirly
