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
#ifndef SWIRLYD_HTTPRESPONSE_HPP
#define SWIRLYD_HTTPRESPONSE_HPP

#include <ostream>

namespace swirly {

class HttpResponseBuf : public std::streambuf {
  public:
    explicit HttpResponseBuf(std::string& buf) noexcept
      : buf_(buf)
    {
    }
    ~HttpResponseBuf() noexcept override;

    // Copy.
    HttpResponseBuf(const HttpResponseBuf& rhs) = delete;
    HttpResponseBuf& operator=(const HttpResponseBuf& rhs) = delete;

    // Move.
    HttpResponseBuf(HttpResponseBuf&&) = delete;
    HttpResponseBuf& operator=(HttpResponseBuf&&) = delete;

    const char_type* data() const noexcept { return buf_.data(); }
    std::streamsize size() const noexcept { return buf_.size(); }
    void reset() noexcept { buf_.clear(); }
    void setContentLength(size_t pos, std::size_t len) noexcept;

  protected:
    int_type overflow(int_type c) noexcept override;

    std::streamsize xsputn(const char_type* s, std::streamsize count) noexcept override;

  private:
    std::string& buf_;
};

class HttpResponse : public std::ostream {
  public:
    explicit HttpResponse(std::string& buf) noexcept
      : std::ostream{nullptr}
      , buf_{buf}
    {
        rdbuf(&buf_);
    }
    ~HttpResponse() noexcept override;

    // Copy.
    HttpResponse(const HttpResponse& rhs) = delete;
    HttpResponse& operator=(const HttpResponse& rhs) = delete;

    // Move.
    HttpResponse(HttpResponse&&) = delete;
    HttpResponse& operator=(HttpResponse&&) = delete;

    const char_type* data() const noexcept { return buf_.data(); }
    std::streamsize size() const noexcept { return buf_.size(); }
    void reset(int status, const char* reason, bool cache = false);
    void setContentLength() noexcept;

  private:
    HttpResponseBuf buf_;
    std::size_t headSize_{0};
    std::size_t lengthAt_{0};
};

} // namespace swirly

#endif // SWIRLYD_HTTPRESPONSE_HPP
