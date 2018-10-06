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
#ifndef SWIRLY_HTTP_REQUEST_HPP
#define SWIRLY_HTTP_REQUEST_HPP

#include <swirly/http/Types.hpp>
#include <swirly/http/Url.hpp>

#include <vector>

namespace swirly {
inline namespace http {

using HttpHeaders = std::vector<std::pair<std::string, std::string>>;

class SWIRLY_API HttpRequest : public BasicUrl<HttpRequest> {
  public:
    HttpRequest() = default;
    ~HttpRequest();

    // Copy.
    HttpRequest(const HttpRequest&) = delete;
    HttpRequest& operator=(const HttpRequest&) = delete;

    // Move.
    HttpRequest(HttpRequest&&) = delete;
    HttpRequest& operator=(HttpRequest&&) = delete;

    HttpMethod method() const noexcept { return method_; }
    const std::string& url() const noexcept { return url_; }
    const HttpHeaders& headers() const noexcept { return headers_; }
    const std::string& body() const noexcept { return body_; }

    void clear() noexcept
    {
        method_ = HttpMethod::Get;
        url_.clear();
        headers_.clear();
        body_.clear();
    }
    void flush() { parse(); }
    void set_method(HttpMethod method) noexcept { method_ = method; }
    void append_url(std::string_view sv) { url_.append(sv.data(), sv.size()); }
    void append_header_field(std::string_view sv, First first)
    {
        if (first == First::Yes) {
            headers_.emplace_back(std::string{sv.data(), sv.size()}, "");
        } else {
            headers_.back().first.append(sv.data(), sv.size());
        }
    }
    void append_header_value(std::string_view sv, First first)
    {
        headers_.back().second.append(sv.data(), sv.size());
    }
    void append_body(std::string_view sv) { body_.append(sv.data(), sv.size()); }

  private:
    HttpMethod method_{HttpMethod::Get};
    std::string url_;
    HttpHeaders headers_;
    std::string body_;
};
} // namespace http
} // namespace swirly

#endif // SWIRLY_HTTP_REQUEST_HPP
