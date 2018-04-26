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
#ifndef SWIRLY_FIX_STREAM_HPP
#define SWIRLY_FIX_STREAM_HPP

#include "Types.hpp"

#include <swirly/sys/Buffer.hpp>

#include <swirly/util/Stream.hpp>

namespace swirly {
inline namespace fix {

class SWIRLY_API FixBuf : public std::streambuf {
  public:
    explicit FixBuf(Buffer& buf) noexcept
    : buf_(buf)
    {
    }
    ~FixBuf() override;

    // Copy.
    FixBuf(const FixBuf& rhs) = delete;
    FixBuf& operator=(const FixBuf& rhs) = delete;

    // Move.
    FixBuf(FixBuf&&) = delete;
    FixBuf& operator=(FixBuf&&) = delete;

    std::streamsize pcount() const noexcept { return pcount_; }
    void commit() noexcept;
    void reset() noexcept
    {
        pbase_ = nullptr;
        pcount_ = 0;
        sum_ = 0;
    }
    void setBodyLength(std::streamsize pos, std::streamsize len) noexcept;

  protected:
    int_type overflow(int_type c) noexcept override;
    std::streamsize xsputn(const char_type* s, std::streamsize count) noexcept override;

  private:
    Buffer& buf_;
    char* pbase_{nullptr};
    std::streamsize pcount_{0};
    std::uint64_t sum_{0};
};

class SWIRLY_API FixStream : public std::ostream {
  public:
    explicit FixStream(Buffer& buf) noexcept
    : std::ostream{nullptr}
    , buf_{buf}
    {
        rdbuf(&buf_);
    }
    ~FixStream() override;

    // Copy.
    FixStream(const FixStream& rhs) = delete;
    FixStream& operator=(const FixStream& rhs) = delete;

    // Move.
    FixStream(FixStream&&) = delete;
    FixStream& operator=(FixStream&&) = delete;

    std::streamsize pcount() const noexcept { return buf_.pcount(); }
    void commit() noexcept;
    void reset() noexcept
    {
        buf_.reset();
        swirly::reset(*this);
        bloff_ = 0;
    }
    void reset(FixVersion ver);

  private:
    FixBuf buf_;
    /**
     * BodyLength offset.
     */
    std::streamsize bloff_{0};
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_STREAM_HPP
