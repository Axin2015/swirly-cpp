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

#include <swirly/util/Exception.hpp>

#include <iosfwd>

namespace swirly {
inline namespace fin {

class SWIRLY_API ServException : public Exception {
  public:
    explicit ServException(std::string_view what) noexcept
    : Exception{what}
    {
    }
    ~ServException() override;

    // Copy.
    ServException(const ServException&) noexcept = default;
    ServException& operator=(const ServException&) noexcept = default;

    // Move.
    ServException(ServException&&) noexcept = default;
    ServException& operator=(ServException&&) noexcept = default;

    static void to_json(int status, const char* reason, const char* detail, std::ostream& os);

    void to_json(std::ostream& os) const { to_json(http_status(), http_reason(), what(), os); }

    virtual int http_status() const noexcept = 0;

    virtual const char* http_reason() const noexcept = 0;
};

inline std::ostream& operator<<(std::ostream& os, const ServException& e)
{
    e.to_json(os);
    return os;
}

/**
 * The request could not be understood by the server due to malformed syntax. The client SHOULD NOT
 * repeat the request without modifications.
 */
class SWIRLY_API BadRequestException : public ServException {
  public:
    explicit BadRequestException(std::string_view what) noexcept
    : ServException{what}
    {
    }
    ~BadRequestException() override;

    // Copy.
    BadRequestException(const BadRequestException&) noexcept = default;
    BadRequestException& operator=(const BadRequestException&) noexcept = default;

    // Move.
    BadRequestException(BadRequestException&&) noexcept = default;
    BadRequestException& operator=(BadRequestException&&) noexcept = default;

    int http_status() const noexcept override;

    const char* http_reason() const noexcept override;
};

class SWIRLY_API AlreadyExistsException : public BadRequestException {
  public:
    explicit AlreadyExistsException(std::string_view what) noexcept
    : BadRequestException{what}
    {
    }
    ~AlreadyExistsException() override;

    // Copy.
    AlreadyExistsException(const AlreadyExistsException&) noexcept = default;
    AlreadyExistsException& operator=(const AlreadyExistsException&) noexcept = default;

    // Move.
    AlreadyExistsException(AlreadyExistsException&&) noexcept = default;
    AlreadyExistsException& operator=(AlreadyExistsException&&) noexcept = default;
};

class SWIRLY_API RefAlreadyExistsException : public AlreadyExistsException {
  public:
    explicit RefAlreadyExistsException(std::string_view what) noexcept
    : AlreadyExistsException{what}
    {
    }
    RefAlreadyExistsException() noexcept = default;

    ~RefAlreadyExistsException() override;

    // Copy.
    RefAlreadyExistsException(const RefAlreadyExistsException&) noexcept = default;
    RefAlreadyExistsException& operator=(const RefAlreadyExistsException&) noexcept = default;

    // Move.
    RefAlreadyExistsException(RefAlreadyExistsException&&) noexcept = default;
    RefAlreadyExistsException& operator=(RefAlreadyExistsException&&) noexcept = default;
};

class SWIRLY_API InvalidException : public BadRequestException {
  public:
    explicit InvalidException(std::string_view what) noexcept
    : BadRequestException{what}
    {
    }
    ~InvalidException() override;

    // Copy.
    InvalidException(const InvalidException&) noexcept = default;
    InvalidException& operator=(const InvalidException&) noexcept = default;

    // Move.
    InvalidException(InvalidException&&) noexcept = default;
    InvalidException& operator=(InvalidException&&) noexcept = default;
};

class SWIRLY_API InvalidLotsException : public InvalidException {
  public:
    explicit InvalidLotsException(std::string_view what) noexcept
    : InvalidException{what}
    {
    }
    ~InvalidLotsException() override;

    // Copy.
    InvalidLotsException(const InvalidLotsException&) noexcept = default;
    InvalidLotsException& operator=(const InvalidLotsException&) noexcept = default;

    // Move.
    InvalidLotsException(InvalidLotsException&&) noexcept = default;
    InvalidLotsException& operator=(InvalidLotsException&&) noexcept = default;
};

class SWIRLY_API InvalidTicksException : public InvalidException {
  public:
    explicit InvalidTicksException(std::string_view what) noexcept
    : InvalidException{what}
    {
    }
    ~InvalidTicksException() override;

    // Copy.
    InvalidTicksException(const InvalidTicksException&) noexcept = default;
    InvalidTicksException& operator=(const InvalidTicksException&) noexcept = default;

    // Move.
    InvalidTicksException(InvalidTicksException&&) noexcept = default;
    InvalidTicksException& operator=(InvalidTicksException&&) noexcept = default;
};

class SWIRLY_API TooLateException : public BadRequestException {
  public:
    explicit TooLateException(std::string_view what) noexcept
    : BadRequestException{what}
    {
    }
    ~TooLateException() override;

    // Copy.
    TooLateException(const TooLateException&) noexcept = default;
    TooLateException& operator=(const TooLateException&) noexcept = default;

    // Move.
    TooLateException(TooLateException&&) noexcept = default;
    TooLateException& operator=(TooLateException&&) noexcept = default;
};

/**
 * The server understood the request, but is refusing to fulfill it. Authorization will not help and
 * the request SHOULD NOT be repeated. If the request method was not HEAD and the server wishes to
 * make public why the request has not been fulfilled, it SHOULD describe the reason for the refusal
 * in the entity. If the server does not wish to make this information available to the client, the
 * status code 404 (Not Found) can be used instead.
 */
class SWIRLY_API ForbiddenException : public ServException {
  public:
    explicit ForbiddenException(std::string_view what) noexcept
    : ServException{what}
    {
    }
    ~ForbiddenException() override;

    // Copy.
    ForbiddenException(const ForbiddenException&) noexcept = default;
    ForbiddenException& operator=(const ForbiddenException&) noexcept = default;

    // Move.
    ForbiddenException(ForbiddenException&&) noexcept = default;
    ForbiddenException& operator=(ForbiddenException&&) noexcept = default;

    int http_status() const noexcept override;

    const char* http_reason() const noexcept override;
};

/**
 * The server encountered an unexpected condition which prevented it from fulfilling the request.
 */
class SWIRLY_API InternalException : public ServException {
  public:
    explicit InternalException(std::string_view what) noexcept
    : ServException{what}
    {
    }
    ~InternalException() override;

    // Copy.
    InternalException(const InternalException&) noexcept = default;
    InternalException& operator=(const InternalException&) noexcept = default;

    // Move.
    InternalException(InternalException&&) noexcept = default;
    InternalException& operator=(InternalException&&) noexcept = default;

    int http_status() const noexcept override;

    const char* http_reason() const noexcept override;
};

/**
 * The method specified in the Request-Line is not allowed for the resource identified by the
 * Request-URI. The response MUST include an Allow header containing a list of valid methods for the
 * requested resource.
 */
class SWIRLY_API MethodNotAllowedException : public ServException {
  public:
    explicit MethodNotAllowedException(std::string_view what) noexcept
    : ServException{what}
    {
    }
    ~MethodNotAllowedException() override;

    // Copy.
    MethodNotAllowedException(const MethodNotAllowedException&) noexcept = default;
    MethodNotAllowedException& operator=(const MethodNotAllowedException&) noexcept = default;

    // Move.
    MethodNotAllowedException(MethodNotAllowedException&&) noexcept = default;
    MethodNotAllowedException& operator=(MethodNotAllowedException&&) noexcept = default;

    int http_status() const noexcept override;

    const char* http_reason() const noexcept override;
};

/**
 * The server has not found anything matching the Request-URI. No indication is given of whether the
 * condition is temporary or permanent. The 410 (Gone) status code SHOULD be used if the server
 * knows, through some internally configurable mechanism, that an old resource is permanently
 * unavailable and has no forwarding address. This status code is commonly used when the server does
 * not wish to reveal exactly why the request has been refused, or when no other response is
 * applicable.
 */
class SWIRLY_API NotFoundException : public ServException {
  public:
    explicit NotFoundException(std::string_view what) noexcept
    : ServException{what}
    {
    }
    ~NotFoundException() override;

    // Copy.
    NotFoundException(const NotFoundException&) noexcept = default;
    NotFoundException& operator=(const NotFoundException&) noexcept = default;

    // Move.
    NotFoundException(NotFoundException&&) noexcept = default;
    NotFoundException& operator=(NotFoundException&&) noexcept = default;

    int http_status() const noexcept override;

    const char* http_reason() const noexcept override;
};

class SWIRLY_API MarketClosedException : public NotFoundException {
  public:
    explicit MarketClosedException(std::string_view what) noexcept
    : NotFoundException{what}
    {
    }
    ~MarketClosedException() override;

    // Copy.
    MarketClosedException(const MarketClosedException&) noexcept = default;
    MarketClosedException& operator=(const MarketClosedException&) noexcept = default;

    // Move.
    MarketClosedException(MarketClosedException&&) noexcept = default;
    MarketClosedException& operator=(MarketClosedException&&) noexcept = default;
};

class SWIRLY_API MarketNotFoundException : public NotFoundException {
  public:
    explicit MarketNotFoundException(std::string_view what) noexcept
    : NotFoundException{what}
    {
    }
    ~MarketNotFoundException() override;

    // Copy.
    MarketNotFoundException(const MarketNotFoundException&) noexcept = default;
    MarketNotFoundException& operator=(const MarketNotFoundException&) noexcept = default;

    // Move.
    MarketNotFoundException(MarketNotFoundException&&) noexcept = default;
    MarketNotFoundException& operator=(MarketNotFoundException&&) noexcept = default;
};

class SWIRLY_API OrderNotFoundException : public NotFoundException {
  public:
    explicit OrderNotFoundException(std::string_view what) noexcept
    : NotFoundException{what}
    {
    }
    ~OrderNotFoundException() override;

    // Copy.
    OrderNotFoundException(const OrderNotFoundException&) noexcept = default;
    OrderNotFoundException& operator=(const OrderNotFoundException&) noexcept = default;

    // Move.
    OrderNotFoundException(OrderNotFoundException&&) noexcept = default;
    OrderNotFoundException& operator=(OrderNotFoundException&&) noexcept = default;
};

/**
 * The server is currently unable to handle the request due to a temporary overloading or
 * maintenance of the server. The implication is that this is a temporary condition which will be
 * alleviated after some delay. If known, the length of the delay MAY be indicated in a Retry-After
 * header. If no Retry-After is given, the client SHOULD handle the response as it would for a 500
 * response.
 */
class SWIRLY_API ServiceUnavailableException : public ServException {
  public:
    explicit ServiceUnavailableException(std::string_view what) noexcept
    : ServException{what}
    {
    }
    ~ServiceUnavailableException() override;

    // Copy.
    ServiceUnavailableException(const ServiceUnavailableException&) noexcept = default;
    ServiceUnavailableException& operator=(const ServiceUnavailableException&) noexcept = default;

    // Move.
    ServiceUnavailableException(ServiceUnavailableException&&) noexcept = default;
    ServiceUnavailableException& operator=(ServiceUnavailableException&&) noexcept = default;

    int http_status() const noexcept override;

    const char* http_reason() const noexcept override;
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
class SWIRLY_API UnauthorizedException : public ServException {
  public:
    explicit UnauthorizedException(std::string_view what) noexcept
    : ServException{what}
    {
    }
    ~UnauthorizedException() override;

    // Copy.
    UnauthorizedException(const UnauthorizedException&) noexcept = default;
    UnauthorizedException& operator=(const UnauthorizedException&) noexcept = default;

    // Move.
    UnauthorizedException(UnauthorizedException&&) noexcept = default;
    UnauthorizedException& operator=(UnauthorizedException&&) noexcept = default;

    int http_status() const noexcept override;

    const char* http_reason() const noexcept override;
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_EXCEPTION_HPP
