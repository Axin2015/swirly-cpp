/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#ifndef SWIRLY_WEB_HTTPHANDLER_HPP
#define SWIRLY_WEB_HTTPHANDLER_HPP

#include <swirly/web/Exception.hpp>

#include <swirly/web/http_parser.h>

#include <experimental/string_view>

namespace std {
using experimental::string_view;
}

namespace swirly {

enum class HttpMethod : int {
    Delete = HTTP_DELETE,
    Get = HTTP_GET,
    Head = HTTP_HEAD,
    Post = HTTP_POST,
    Put = HTTP_PUT,
    Connect = HTTP_CONNECT,
    Options = HTTP_OPTIONS,
    Trace = HTTP_TRACE,
    Copy = HTTP_COPY,
    Lock = HTTP_LOCK,
    MkCol = HTTP_MKCOL,
    Move = HTTP_MOVE,
    PropFind = HTTP_PROPFIND,
    PropPatch = HTTP_PROPPATCH,
    Search = HTTP_SEARCH,
    Unlock = HTTP_UNLOCK,
    Bind = HTTP_BIND,
    Rebind = HTTP_REBIND,
    Unbind = HTTP_UNBIND,
    Acl = HTTP_ACL,
    Report = HTTP_REPORT,
    MkActivity = HTTP_MKACTIVITY,
    Checkout = HTTP_CHECKOUT,
    Merge = HTTP_MERGE,
    MSearch = HTTP_MSEARCH,
    Notify = HTTP_NOTIFY,
    Subscribe = HTTP_SUBSCRIBE,
    Unsubscribe = HTTP_UNSUBSCRIBE,
    Patch = HTTP_PATCH,
    Purge = HTTP_PURGE,
    MkCalendar = HTTP_MKCALENDAR,
    Link = HTTP_LINK,
    Unlink = HTTP_UNLINK
};

inline const char* enumString(HttpMethod method)
{
    return http_method_str(static_cast<http_method>(method));
}

inline std::ostream& operator<<(std::ostream& os, HttpMethod method)
{
    return os << enumString(method);
}

enum class HttpType : int { Request = HTTP_REQUEST, Response = HTTP_RESPONSE };

template <typename DerivedT>
class BasicHttpHandler {
  public:
    explicit BasicHttpHandler(HttpType type) noexcept : type_{type}
    {
        // The http_parser_init() function preserves "data".
        // Important: cast is required for CRTP to work correctly with multiple inheritance.
        parser_.data = static_cast<DerivedT*>(this);
        http_parser_init(&parser_, static_cast<http_parser_type>(type));
        lastHeaderElem_ = None;
    }

    // Copy.
    BasicHttpHandler(const BasicHttpHandler&) noexcept = default;
    BasicHttpHandler& operator=(const BasicHttpHandler&) noexcept = default;

    // Move.
    BasicHttpHandler(BasicHttpHandler&&) noexcept = default;
    BasicHttpHandler& operator=(BasicHttpHandler&&) noexcept = default;

    int httpMajor() const noexcept { return parser_.http_major; }
    int httpMinor() const noexcept { return parser_.http_minor; }
    int statusCode() const noexcept { return parser_.status_code; }
    HttpMethod method() const noexcept { return static_cast<HttpMethod>(parser_.method); }
    bool shouldKeepAlive() const noexcept { return http_should_keep_alive(&parser_) != 0; }
    bool bodyIsFinal() const noexcept { return http_body_is_final(&parser_) != 0; }

    void pause() noexcept { http_parser_pause(&parser_, 1); }

  protected:
    ~BasicHttpHandler() noexcept = default;

    void reset() noexcept
    {
        // The http_parser_init() function preserves "data".
        http_parser_init(&parser_, static_cast<http_parser_type>(type_));
        lastHeaderElem_ = None;
    }
    size_t parse(std::string_view buf)
    {
        static http_parser_settings settings{makeSettings()};
        const auto rc = http_parser_execute(&parser_, &settings, buf.data(), buf.size());
        const auto err = static_cast<http_errno>(parser_.http_errno);
        if (err != HPE_OK) {
            if (err == HPE_PAUSED) {
                // Clear pause state.
                http_parser_pause(&parser_, 0);
            } else {
                throw ParseException{errMsg() << http_errno_name(err) << ": "
                                              << http_errno_description(err)};
            }
        }
        return rc;
    }

  private:
    static http_parser_settings makeSettings() noexcept
    {
        http_parser_settings settings{};
        settings.on_message_begin = cbMessageBegin;
        settings.on_url = cbUrl;
        settings.on_status = cbStatus;
        settings.on_header_field = cbHeaderField;
        settings.on_header_value = cbHeaderValue;
        settings.on_headers_complete = cbHeadersEnd;
        settings.on_body = cbBody;
        settings.on_message_complete = cbMessageEnd;
        settings.on_chunk_header = cbChunkHeader;
        settings.on_chunk_complete = cbChunkEnd;
        return settings;
    }
    static int cbMessageBegin(http_parser* parser) noexcept
    {
        return static_cast<DerivedT*>(parser->data)->onMessageBegin() ? 0 : -1;
    }
    static int cbUrl(http_parser* parser, const char* at, size_t length) noexcept
    {
        return static_cast<DerivedT*>(parser->data)->onUrl({at, length}) ? 0 : -1;
    }
    static int cbStatus(http_parser* parser, const char* at, size_t length) noexcept
    {
        return static_cast<DerivedT*>(parser->data)->onStatus({at, length}) ? 0 : -1;
    }
    static int cbHeaderField(http_parser* parser, const char* at, size_t length) noexcept
    {
        auto* const obj = static_cast<DerivedT*>(parser->data);
        bool first;
        if (obj->lastHeaderElem_ != Field) {
            obj->lastHeaderElem_ = Field;
            first = true;
        } else {
            first = false;
        }
        return obj->onHeaderField({at, length}, first) ? 0 : -1;
    }
    static int cbHeaderValue(http_parser* parser, const char* at, size_t length) noexcept
    {
        auto* const obj = static_cast<DerivedT*>(parser->data);
        bool first;
        if (obj->lastHeaderElem_ != Value) {
            obj->lastHeaderElem_ = Value;
            first = true;
        } else {
            first = false;
        }
        return obj->onHeaderValue({at, length}, first) ? 0 : -1;
    }
    static int cbHeadersEnd(http_parser* parser) noexcept
    {
        return static_cast<DerivedT*>(parser->data)->onHeadersEnd() ? 0 : -1;
    }
    static int cbBody(http_parser* parser, const char* at, size_t length) noexcept
    {
        return static_cast<DerivedT*>(parser->data)->onBody({at, length}) ? 0 : -1;
    }
    static int cbMessageEnd(http_parser* parser) noexcept
    {
        return static_cast<DerivedT*>(parser->data)->onMessageEnd() ? 0 : -1;
    }
    static int cbChunkHeader(http_parser* parser) noexcept
    {
        // When cb_chunk_header is called, the current chunk length is stored in parser->content_length.
        return static_cast<DerivedT*>(parser->data)->onChunkHeader(parser->content_length) ? 0 : -1;
    }
    static int cbChunkEnd(http_parser* parser) noexcept
    {
        return static_cast<DerivedT*>(parser->data)->onChunkEnd() ? 0 : -1;
    }
    HttpType type_;
    http_parser parser_;
    enum { None = 0, Field, Value } lastHeaderElem_;
};

} // namespace swirly

#endif // SWIRLY_WEB_HTTPHANDLER_HPP
