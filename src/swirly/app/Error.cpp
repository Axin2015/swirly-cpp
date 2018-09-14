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
#include "Error.hpp"

namespace swirly {
using namespace std::string_literals;
inline namespace app {
namespace {
class ErrorCategory : public std::error_category {
  public:
    constexpr ErrorCategory() noexcept = default;
    ~ErrorCategory() override = default;

    // Copy.
    ErrorCategory(const ErrorCategory&) = delete;
    ErrorCategory& operator=(const ErrorCategory&) = delete;

    // Move.
    ErrorCategory(ErrorCategory&&) = delete;
    ErrorCategory& operator=(ErrorCategory&&) = delete;

    const char* name() const noexcept override { return "swirly"; }
    std::string message(int err) const override
    {
        std::string msg;
        switch (static_cast<Error>(err)) {
        case Error::BadRequest:
            msg = "bad request"s;
            break;
        case Error::AlreadyExists:
            msg = "already exists"s;
            break;
        case Error::RefAlreadyExists:
            msg = "reference already exists"s;
            break;
        case Error::Invalid:
            msg = "invalid"s;
            break;
        case Error::InvalidLots:
            msg = "invalid lots"s;
            break;
        case Error::InvalidTicks:
            msg = "invalid ticks"s;
            break;
        case Error::ProtocolError:
            msg = "protocol error"s;
            break;
        case Error::TooLate:
            msg = "tool late"s;
            break;
        case Error::Unauthorized:
            msg = "unauthorized"s;
            break;
        case Error::Forbidden:
            msg = "forbidden"s;
            break;
        case Error::NotFound:
            msg = "not found"s;
            break;
        case Error::AccntNotFound:
            msg = "account not found"s;
            break;
        case Error::MarketNotFound:
            msg = "market not found"s;
            break;
        case Error::OrderNotFound:
            msg = "order not found"s;
            break;
        case Error::MethodNotAllowed:
            msg = "method not allowed"s;
            break;
        case Error::InternalError:
            msg = "internal error"s;
            break;
        case Error::DatabaseError:
            msg = "database error"s;
            break;
        case Error::ServiceUnavailable:
            msg = "service unavailable"s;
            break;
        case Error::MarketClosed:
            msg = "market closed"s;
            break;
        default:
            msg = "unknown error"s;
        }
        return msg;
    }

  private:
};

const ErrorCategory ecat_{};
} // namespace

const std::error_category& error_category() noexcept
{
    return ecat_;
}

std::error_code make_error_code(Error err)
{
    return {static_cast<int>(err), ecat_};
}

} // namespace app
} // namespace swirly
