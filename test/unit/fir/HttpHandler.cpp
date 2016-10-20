/*
 * The Restful Matching-Engine.
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
#include <swirly/fir/HttpHandler.hpp>

#include <swirly/fir/Url.hpp>

#include <swirly/ash/String.hpp>

#include <swirly/tea/Test.hpp>

#include <iostream>
#include <vector>

using namespace std;
using namespace swirly;

namespace {

class HttpHandler : public BasicHttpHandler<HttpHandler>, public BasicUrl<HttpHandler> {
  friend class BasicHttpHandler<HttpHandler>;
  friend class BasicUrl<HttpHandler>;

 public:
  using BasicHttpHandler<HttpHandler>::BasicHttpHandler;
  ~HttpHandler() noexcept = default;

  const auto& url() const noexcept { return url_; }
  const auto& status() const noexcept { return status_; }
  const auto& headers() const noexcept { return headers_; }
  const auto& body() const noexcept { return body_; }

  void clear() noexcept
  {
    url_.clear();
    status_.clear();
    headers_.clear();
    body_.clear();
  }
  using BasicHttpHandler<HttpHandler>::parse;

 private:
  bool onMessageBegin() noexcept
  {
    BasicUrl<HttpHandler>::reset();
    clear();
    return true;
  }
  bool onUrl(string_view sv) noexcept
  {
    url_.append(sv.data(), sv.size());
    return true;
  }
  bool onStatus(string_view sv) noexcept
  {
    status_.append(sv.data(), sv.size());
    return true;
  }
  bool onHeaderField(string_view sv, bool create) noexcept
  {
    if (create) {
      headers_.emplace_back(string{sv.data(), sv.size()}, "");
    } else {
      headers_.back().first.append(sv.data(), sv.size());
    }
    return true;
  }
  bool onHeaderValue(string_view sv) noexcept
  {
    headers_.back().second.append(sv.data(), sv.size());
    return true;
  }
  bool onHeadersEnd() noexcept { return true; }
  bool onBody(string_view sv) noexcept
  {
    body_.append(sv.data(), sv.size());
    return true;
  }
  bool onMessageEnd() noexcept
  {
    BasicUrl<HttpHandler>::parse();
    return true;
  }
  bool onChunkHeader(size_t len) noexcept { return true; }
  bool onChunkEnd() noexcept { return true; }

  string url_;
  string status_;
  vector<pair<string, string>> headers_;
  string body_;
};

} // anonymous

SWIRLY_TEST_CASE(HttpInitialRequestLine)
{
  constexpr auto Message = //
    "GET /path/to/file/index.html HTTP/1.0\r\n" //
    "\r\n"_sv;

  HttpHandler h{HttpType::Request};
  SWIRLY_CHECK(h.parse(Message) == Message.size());
  SWIRLY_CHECK(!h.shouldKeepAlive());
  SWIRLY_CHECK(h.httpMajor() == 1);
  SWIRLY_CHECK(h.httpMinor() == 0);

  SWIRLY_CHECK(h.method() == HttpMethod::Get);
  SWIRLY_CHECK(h.url() == "/path/to/file/index.html"s);

  SWIRLY_CHECK(h.headers().empty());
  SWIRLY_CHECK(h.body().empty());
}

SWIRLY_TEST_CASE(HttpInitialResponseLine)
{
  constexpr auto Message = //
    "HTTP/1.0 404 Not Found\r\n" //
    "\r\n"_sv;

  HttpHandler h{HttpType::Response};
  SWIRLY_CHECK(h.parse(Message) == Message.size());
  SWIRLY_CHECK(!h.shouldKeepAlive());
  SWIRLY_CHECK(h.httpMajor() == 1);
  SWIRLY_CHECK(h.httpMinor() == 0);

  SWIRLY_CHECK(h.statusCode() == 404);
  SWIRLY_CHECK(h.status() == "Not Found"s);

  SWIRLY_CHECK(h.headers().empty());
  SWIRLY_CHECK(h.body().empty());
}

SWIRLY_TEST_CASE(HttpBasicRequest)
{
  constexpr auto Message = //
    "GET /path/file.html HTTP/1.0\r\n" //
    "From: someuser@swirlycloud.com\r\n" //
    "User-Agent: HTTPTool/1.0\r\n" //
    "\r\n"_sv;

  HttpHandler h{HttpType::Request};
  SWIRLY_CHECK(h.parse(Message) == Message.size());
  SWIRLY_CHECK(!h.shouldKeepAlive());
  SWIRLY_CHECK(h.httpMajor() == 1);
  SWIRLY_CHECK(h.httpMinor() == 0);

  SWIRLY_CHECK(h.method() == HttpMethod::Get);
  SWIRLY_CHECK(h.url() == "/path/file.html"s);

  SWIRLY_CHECK(h.headers().size() == 2);
  SWIRLY_CHECK(h.headers()[0] == make_pair("From"s, "someuser@swirlycloud.com"s));
  SWIRLY_CHECK(h.headers()[1] == make_pair("User-Agent"s, "HTTPTool/1.0"s));

  SWIRLY_CHECK(h.body().empty());
}

SWIRLY_TEST_CASE(HttpBasicResponse)
{
  constexpr auto Message = //
    "HTTP/1.0 200 OK\r\n" //
    "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n" //
    "Content-Type: text/plain\r\n" //
    "Content-Length: 13\r\n" //
    "\r\n" //
    "Hello, World!"_sv;

  HttpHandler h{HttpType::Response};
  SWIRLY_CHECK(h.parse(Message) == Message.size());
  SWIRLY_CHECK(!h.shouldKeepAlive());
  SWIRLY_CHECK(h.httpMajor() == 1);
  SWIRLY_CHECK(h.httpMinor() == 0);

  SWIRLY_CHECK(h.statusCode() == 200);
  SWIRLY_CHECK(h.status() == "OK"s);

  SWIRLY_CHECK(h.headers().size() == 3);
  SWIRLY_CHECK(h.headers()[0] == make_pair("Date"s, "Fri, 31 Dec 1999 23:59:59 GMT"s));
  SWIRLY_CHECK(h.headers()[1] == make_pair("Content-Type"s, "text/plain"s));
  SWIRLY_CHECK(h.headers()[2] == make_pair("Content-Length"s, "13"s));

  SWIRLY_CHECK(h.body() == "Hello, World!"s);
}

SWIRLY_TEST_CASE(HttpPostRequest)
{
  constexpr auto Message = //
    "POST /path/script.cgi HTTP/1.0\r\n" //
    "From: frog@swirlycloud.com\r\n" //
    "User-Agent: HTTPTool/1.0\r\n" //
    "Content-Type: application/x-www-form-urlencoded\r\n" //
    "Content-Length: 32\r\n" //
    "\r\n" //
    "home=Cosby&favorite+flavor=flies"_sv;

  HttpHandler h{HttpType::Request};
  SWIRLY_CHECK(h.parse(Message) == Message.size());
  SWIRLY_CHECK(!h.shouldKeepAlive());
  SWIRLY_CHECK(h.httpMajor() == 1);
  SWIRLY_CHECK(h.httpMinor() == 0);

  SWIRLY_CHECK(h.method() == HttpMethod::Post);
  SWIRLY_CHECK(h.url() == "/path/script.cgi"s);

  SWIRLY_CHECK(h.headers().size() == 4);
  SWIRLY_CHECK(h.headers()[0] == make_pair("From"s, "frog@swirlycloud.com"s));
  SWIRLY_CHECK(h.headers()[1] == make_pair("User-Agent"s, "HTTPTool/1.0"s));
  SWIRLY_CHECK(h.headers()[2] == make_pair("Content-Type"s, "application/x-www-form-urlencoded"s));
  SWIRLY_CHECK(h.headers()[3] == make_pair("Content-Length"s, "32"s));

  SWIRLY_CHECK(h.body() == "home=Cosby&favorite+flavor=flies"s);
}

SWIRLY_TEST_CASE(HttpKeepAliveRequest)
{
  constexpr auto Message = //
    "GET /path/file.html HTTP/1.1\r\n" //
    "Host: www.host1.com:80\r\n" //
    "\r\n"_sv;

  HttpHandler h{HttpType::Request};
  SWIRLY_CHECK(h.parse(Message) == Message.size());
  SWIRLY_CHECK(h.shouldKeepAlive());
  SWIRLY_CHECK(h.httpMajor() == 1);
  SWIRLY_CHECK(h.httpMinor() == 1);

  SWIRLY_CHECK(h.method() == HttpMethod::Get);
  SWIRLY_CHECK(h.url() == "/path/file.html"s);

  SWIRLY_CHECK(h.headers().size() == 1);
  SWIRLY_CHECK(h.headers()[0] == make_pair("Host"s, "www.host1.com:80"s));

  SWIRLY_CHECK(h.body().empty());
}

SWIRLY_TEST_CASE(HttpChunkedResponse)
{
  constexpr auto Message = //
    "HTTP/1.1 200 OK\r\n" //
    "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n" //
    "Content-Type: text/plain\r\n" //
    "Transfer-Encoding: chunked\r\n" //
    "\r\n" //
    "1a; ignore-stuff-here\r\n" //
    "abcdefghijklmnopqrstuvwxyz\r\n" //
    "10\r\n" //
    "1234567890abcdef\r\n" //
    "0\r\n" //
    "some-footer: some-value\r\n" //
    "another-footer: another-value\r\n" //
    "\r\n"_sv;

  HttpHandler h{HttpType::Response};
  SWIRLY_CHECK(h.parse(Message) == Message.size());
  SWIRLY_CHECK(h.shouldKeepAlive());
  SWIRLY_CHECK(h.httpMajor() == 1);
  SWIRLY_CHECK(h.httpMinor() == 1);

  SWIRLY_CHECK(h.statusCode() == 200);
  SWIRLY_CHECK(h.status() == "OK"s);

  SWIRLY_CHECK(h.headers().size() == 5);
  SWIRLY_CHECK(h.headers()[0] == make_pair("Date"s, "Fri, 31 Dec 1999 23:59:59 GMT"s));
  SWIRLY_CHECK(h.headers()[1] == make_pair("Content-Type"s, "text/plain"s));
  SWIRLY_CHECK(h.headers()[2] == make_pair("Transfer-Encoding"s, "chunked"s));
  SWIRLY_CHECK(h.headers()[3] == make_pair("some-footer"s, "some-value"s));
  SWIRLY_CHECK(h.headers()[4] == make_pair("another-footer"s, "another-value"s));

  SWIRLY_CHECK(h.body() == "abcdefghijklmnopqrstuvwxyz1234567890abcdef"s);
}

SWIRLY_TEST_CASE(HttpMultiResponse)
{
  constexpr auto Message = //
    "POST /path/script.cgi HTTP/1.1\r\n" //
    "Content-Type: text/plain\r\n" //
    "Content-Length: 5\r\n" //
    "\r\n" //
    "first" //
    "POST /path/script.cgi HTTP/1.1\r\n" //
    "Content-Type: text/plain\r\n" //
    "Content-Length: 6\r\n" //
    "\r\n" //
    "second"_sv;

  HttpHandler h{HttpType::Request};
  SWIRLY_CHECK(h.parse(Message) == Message.size());
  SWIRLY_CHECK(h.shouldKeepAlive());
  SWIRLY_CHECK(h.httpMajor() == 1);
  SWIRLY_CHECK(h.httpMinor() == 1);

  SWIRLY_CHECK(h.method() == HttpMethod::Post);
  SWIRLY_CHECK(h.url() == "/path/script.cgi"s);

  SWIRLY_CHECK(h.headers().size() == 2);
  SWIRLY_CHECK(h.headers()[0] == make_pair("Content-Type"s, "text/plain"s));
  SWIRLY_CHECK(h.headers()[1] == make_pair("Content-Length"s, "6"s));

  SWIRLY_CHECK(h.body() == "second"s);
}
