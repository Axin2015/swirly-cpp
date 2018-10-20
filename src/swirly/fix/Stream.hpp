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

#include <swirly/fix/Types.hpp>

#include <swirly/sys/Buffer.hpp>

#include <swirly/util/Stream.hpp>

#include <charconv>

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
    FixBuf(const FixBuf&) = delete;
    FixBuf& operator=(const FixBuf&) = delete;

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
    void set_body_length(std::streamsize pos, std::streamsize len) noexcept;

    void put_num(int val) { put_num<int, 11>(val); }
    void put_num(std::int64_t val) { put_num<std::int64_t, 21>(val); }

  protected:
    int_type overflow(int_type c) override;
    std::streamsize xsputn(const char_type* s, std::streamsize count) override;

  private:
    template <typename ValueT, std::size_t MaxN>
    void put_num(ValueT val)
    {
        auto buf = buf_.prepare(pcount_ + MaxN + CheckSumLen);
        pbase_ = buffer_cast<char*>(buf);
        char* it = pbase_ + pcount_;
        const auto [end, ec] = std::to_chars(it, it + MaxN, val);
        pcount_ += (end - it);
        for (; it != end; ++it) {
            sum_ += *it;
        }
    }
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
    FixStream(const FixStream&) = delete;
    FixStream& operator=(const FixStream&) = delete;

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
    void reset(Version ver);

    template <typename ValueT>
    void put_num(ValueT val)
    {
        buf_.put_num(val);
    }

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
