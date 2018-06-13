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
#ifndef SWIRLY_FIN_EXCEPTION_HPP
#define SWIRLY_FIN_EXCEPTION_HPP

#include <swirly/fin/Error.hpp>

#include <swirly/util/Exception.hpp>

namespace swirly {
inline namespace fin {

/**
 * The request could not be understood by the server due to malformed syntax. The client SHOULD NOT
 * repeat the request without modifications.
 */
struct SWIRLY_API BadRequestException : Exception {
    BadRequestException()
    : Exception{Error::BadRequest}
    {
    }
    explicit BadRequestException(std::string_view what)
    : Exception{Error::BadRequest, what}
    {
    }
    ~BadRequestException() override;

  protected:
    using Exception::Exception;
};

struct SWIRLY_API AlreadyExistsException : BadRequestException {
    AlreadyExistsException()
    : BadRequestException{Error::AlreadyExists}
    {
    }
    explicit AlreadyExistsException(std::string_view what)
    : BadRequestException{Error::AlreadyExists, what}
    {
    }
    ~AlreadyExistsException() override;

  protected:
    using BadRequestException::BadRequestException;
};

struct SWIRLY_API RefAlreadyExistsException : AlreadyExistsException {
    RefAlreadyExistsException()
    : AlreadyExistsException{Error::RefAlreadyExists}
    {
    }
    explicit RefAlreadyExistsException(std::string_view what)
    : AlreadyExistsException{Error::RefAlreadyExists, what}
    {
    }
    ~RefAlreadyExistsException() override;

  protected:
    using AlreadyExistsException::AlreadyExistsException;
};

struct SWIRLY_API InvalidException : BadRequestException {
    InvalidException()
    : BadRequestException{Error::Invalid}
    {
    }
    explicit InvalidException(std::string_view what)
    : BadRequestException{Error::Invalid, what}
    {
    }
    ~InvalidException() override;

  protected:
    using BadRequestException::BadRequestException;
};

struct SWIRLY_API InvalidLotsException : InvalidException {
    InvalidLotsException()
    : InvalidException{Error::InvalidLots}
    {
    }
    explicit InvalidLotsException(std::string_view what)
    : InvalidException{Error::InvalidLots, what}
    {
    }
    ~InvalidLotsException() override;

  protected:
    using InvalidException::InvalidException;
};

struct SWIRLY_API InvalidTicksException : InvalidException {
    InvalidTicksException()
    : InvalidException{Error::InvalidTicks}
    {
    }
    explicit InvalidTicksException(std::string_view what)
    : InvalidException{Error::InvalidTicks, what}
    {
    }
    ~InvalidTicksException() override;

  protected:
    using InvalidException::InvalidException;
};

struct SWIRLY_API ProtocolException : BadRequestException {
    ProtocolException()
    : BadRequestException{Error::ProtocolError}
    {
    }
    explicit ProtocolException(std::string_view what)
    : BadRequestException{Error::ProtocolError, what}
    {
    }
    ~ProtocolException() override;

  protected:
    using BadRequestException::BadRequestException;
};

struct SWIRLY_API TooLateException : BadRequestException {
    TooLateException()
    : BadRequestException{Error::TooLate}
    {
    }
    explicit TooLateException(std::string_view what)
    : BadRequestException{Error::TooLate, what}
    {
    }
    ~TooLateException() override;

  protected:
    using BadRequestException::BadRequestException;
};

/**
 * The request requires user authentication. The response MUST include a WWW-Authenticate header
 * field (section 14.47) containing a challenge applicable to the requested resource. The client MAY
 * repeat the request with a suitable Authorization header field (section 14.8). If the request
 * already included Authorization credentials, then the 401 response indicates that authorization
 * has been refused for those credentials. If the 401 response contains the same challenge as the
 * prior response, and the user agent has already attempted authentication at least once, then the
 * user SHOULD be presented the entity that was given in the response, since that entity might
 * include relevant diagnostic information. HTTP access authentication is explained in "HTTP
 * Authentication: Basic and Digest Access Authentication".
 */
struct SWIRLY_API UnauthorizedException : Exception {
    UnauthorizedException()
    : Exception{Error::Unauthorized}
    {
    }
    explicit UnauthorizedException(std::string_view what)
    : Exception{Error::Unauthorized, what}
    {
    }
    ~UnauthorizedException() override;

  protected:
    using Exception::Exception;
};

/**
 * The server understood the request, but is refusing to fulfill it. Authorization will not help and
 * the request SHOULD NOT be repeated. If the request method was not HEAD and the server wishes to
 * make public why the request has not been fulfilled, it SHOULD describe the reason for the refusal
 * in the entity. If the server does not wish to make this information available to the client, the
 * status code 404 (Not Found) can be used instead.
 */
struct SWIRLY_API ForbiddenException : Exception {
    ForbiddenException()
    : Exception{Error::Forbidden}
    {
    }
    explicit ForbiddenException(std::string_view what)
    : Exception{Error::Forbidden, what}
    {
    }
    ~ForbiddenException() override;

  protected:
    using Exception::Exception;
};

/**
 * The server has not found anything matching the Request-URI. No indication is given of whether the
 * condition is temporary or permanent. The 410 (Gone) status code SHOULD be used if the server
 * knows, through some internally configurable mechanism, that an old resource is permanently
 * unavailable and has no forwarding address. This status code is commonly used when the server does
 * not wish to reveal exactly why the request has been refused, or when no other response is
 * applicable.
 */
struct SWIRLY_API NotFoundException : Exception {
    NotFoundException()
    : Exception{Error::NotFound}
    {
    }
    explicit NotFoundException(std::string_view what)
    : Exception{Error::NotFound, what}
    {
    }
    ~NotFoundException() override;

  protected:
    using Exception::Exception;
};

struct SWIRLY_API AccntNotFoundException : NotFoundException {
    AccntNotFoundException()
    : NotFoundException{Error::AccntNotFound}
    {
    }
    explicit AccntNotFoundException(std::string_view what)
    : NotFoundException{Error::AccntNotFound, what}
    {
    }
    ~AccntNotFoundException() override;

  protected:
    using NotFoundException::NotFoundException;
};

struct SWIRLY_API MarketNotFoundException : NotFoundException {
    MarketNotFoundException()
    : NotFoundException{Error::MarketNotFound}
    {
    }
    explicit MarketNotFoundException(std::string_view what)
    : NotFoundException{Error::MarketNotFound, what}
    {
    }
    ~MarketNotFoundException() override;

  protected:
    using NotFoundException::NotFoundException;
};

struct SWIRLY_API OrderNotFoundException : NotFoundException {
    OrderNotFoundException()
    : NotFoundException{Error::OrderNotFound}
    {
    }
    explicit OrderNotFoundException(std::string_view what)
    : NotFoundException{Error::OrderNotFound, what}
    {
    }
    ~OrderNotFoundException() override;

  protected:
    using NotFoundException::NotFoundException;
};

/**
 * The method specified in the Request-Line is not allowed for the resource identified by the
 * Request-URI. The response MUST include an Allow header containing a list of valid methods for the
 * requested resource.
 */
struct SWIRLY_API MethodNotAllowedException : Exception {
    MethodNotAllowedException()
    : Exception{Error::MethodNotAllowed}
    {
    }
    explicit MethodNotAllowedException(std::string_view what)
    : Exception{Error::MethodNotAllowed, what}
    {
    }
    ~MethodNotAllowedException() override;

  protected:
    using Exception::Exception;
};

/**
 * The server encountered an unexpected condition which prevented it from fulfilling the request.
 */
struct SWIRLY_API InternalException : Exception {
    InternalException()
    : Exception{Error::InternalError}
    {
    }
    explicit InternalException(std::string_view what)
    : Exception{Error::InternalError, what}
    {
    }
    ~InternalException() override;

  protected:
    using Exception::Exception;
};

struct SWIRLY_API DatabaseException : InternalException {
    DatabaseException()
    : InternalException{Error::DatabaseError}
    {
    }
    explicit DatabaseException(std::string_view what)
    : InternalException{Error::DatabaseError, what}
    {
    }
    ~DatabaseException() override;

  protected:
    using InternalException::InternalException;
};

/**
 * The server is currently unable to handle the request due to a temporary overloading or
 * maintenance of the server. The implication is that this is a temporary condition which will be
 * alleviated after some delay. If known, the length of the delay MAY be indicated in a Retry-After
 * header. If no Retry-After is given, the client SHOULD handle the response as it would for a 500
 * response.
 */
struct SWIRLY_API ServiceUnavailableException : Exception {
    ServiceUnavailableException()
    : Exception{Error::ServiceUnavailable}
    {
    }
    explicit ServiceUnavailableException(std::string_view what)
    : Exception{Error::ServiceUnavailable, what}
    {
    }
    ~ServiceUnavailableException() override;

  protected:
    using Exception::Exception;
};

struct SWIRLY_API MarketClosedException : ServiceUnavailableException {
    MarketClosedException()
    : ServiceUnavailableException{Error::MarketClosed}
    {
    }
    explicit MarketClosedException(std::string_view what)
    : ServiceUnavailableException{Error::MarketClosed, what}
    {
    }
    ~MarketClosedException() override;

  protected:
    using ServiceUnavailableException::ServiceUnavailableException;
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_EXCEPTION_HPP
