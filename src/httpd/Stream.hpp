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
#ifndef SWIRLY_HTTPD_STREAM_HPP
#define SWIRLY_HTTPD_STREAM_HPP

#include "mongoose.h"

#include <ostream>

namespace swirly {
namespace mg {

/**
 * @addtogroup App
 * @{
 */

class StreamBuf : public std::streambuf {
    mbuf buf_;

 protected:
    int_type overflow(int_type c) noexcept override;

    std::streamsize xsputn(const char_type* s, std::streamsize count) noexcept override;

 public:
    StreamBuf() noexcept
    {
        mbuf_init(&buf_, 4096);
    }
    ~StreamBuf() noexcept override;

    // Copy.
    StreamBuf(const StreamBuf& rhs) = delete;
    StreamBuf& operator=(const StreamBuf& rhs) = delete;

    // Move.
    StreamBuf(StreamBuf&&) = delete;
    StreamBuf& operator=(StreamBuf&&) = delete;

    void reset() noexcept
    {
        buf_.len = 0;
    }
    const char_type* data() const noexcept
    {
        return buf_.buf;
    }
    std::streamsize size() const noexcept
    {
        return buf_.len;
    }
};

class OStream : public std::ostream {
    StreamBuf buf_;

 public:
    OStream() : std::ostream{nullptr}
    {
        rdbuf(&buf_);
    }
    ~OStream() noexcept override;

    // Copy.
    OStream(const OStream& rhs) = delete;
    OStream& operator=(const OStream& rhs) = delete;

    // Move.
    OStream(OStream&&) = delete;
    OStream& operator=(OStream&&) = delete;

    void reset() noexcept
    {
        buf_.reset();
    }
    const char_type* data() const noexcept
    {
        return buf_.data();
    }
    std::streamsize size() const noexcept
    {
        return buf_.size();
    }
};

/** @} */

} // mg
} // swirly

#endif // SWIRLY_HTTPD_STREAM_HPP
