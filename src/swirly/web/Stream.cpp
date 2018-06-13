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
#include "Stream.hpp"

namespace swirly {
inline namespace web {
using namespace std;
namespace {

// All 1xx (informational), 204 (no content), and 304 (not modified) responses must not include a
// body.
constexpr bool with_body(HttpStatus status) noexcept
{
    const auto n = static_cast<int>(status);
    return !((n >= 100 && n < 200) || n == 204 || n == 304);
}

} // namespace

void HttpBuf::set_content_length(std::streamsize pos, std::streamsize len) noexcept
{
    auto it = pbase_ + pos;
    do {
        --it;
        *it = '0' + len % 10;
        len /= 10;
    } while (len > 0);
}

HttpBuf::~HttpBuf() = default;

HttpBuf::int_type HttpBuf::overflow(int_type c) noexcept
{
    if (c != traits_type::eof()) {
        auto buf = buf_.prepare(pcount_ + 1);
        pbase_ = buffer_cast<char*>(buf);
        pbase_[pcount_++] = c;
    }
    return c;
}

streamsize HttpBuf::xsputn(const char_type* s, streamsize count) noexcept
{
    auto buf = buf_.prepare(pcount_ + count);
    pbase_ = buffer_cast<char*>(buf);
    memcpy(pbase_ + pcount_, s, count);
    pcount_ += count;
    return count;
}

HttpStream::~HttpStream() = default;

void HttpStream::commit() noexcept
{
    if (cloff_ > 0) {
        buf_.set_content_length(cloff_, pcount() - hcount_);
    }
    buf_.commit();
}

void HttpStream::reset(HttpStatus status, const char* reason, bool cache)
{
    buf_.reset();
    swirly::reset(*this);

    *this << "HTTP/1.1 "sv << status << ' ' << reason;
    if (!cache) {
        *this << "\r\nCache-Control: no-cache"sv;
    }
    if (with_body(status)) {
        // Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF. Use 10 space
        // place-holder for content length. RFC2616 states that field value MAY be preceded by any
        // amount of LWS, though a single SP is preferred.
        *this <<                                 //
            "\r\nContent-Type: application/json" //
            "\r\nContent-Length:          0";
        cloff_ = pcount();
    } else {
        cloff_ = 0;
    }
    *this << "\r\n\r\n"sv;
    hcount_ = pcount();
}

} // namespace web
} // namespace swirly
