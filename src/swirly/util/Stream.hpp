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
#ifndef SWIRLY_UTIL_STREAM_HPP
#define SWIRLY_UTIL_STREAM_HPP

#include <swirly/Config.h>

#include <experimental/iterator>

#include <ostream>
#include <string_view>

namespace swirly {
inline namespace util {

SWIRLY_API void reset(std::ostream& os) noexcept;

template <std::size_t MaxN>
class StaticBuf : public std::streambuf {
  public:
    StaticBuf() noexcept { reset(); }
    ~StaticBuf() override = default;

    // Copy.
    StaticBuf(const StaticBuf& rhs) = delete;
    StaticBuf& operator=(const StaticBuf& rhs) = delete;

    // Move.
    StaticBuf(StaticBuf&&) = delete;
    StaticBuf& operator=(StaticBuf&&) = delete;

    const char* data() const noexcept { return pbase(); }
    bool empty() const noexcept { return pbase() == pptr(); }
    std::size_t size() const noexcept { return pptr() - pbase(); }
    std::string_view str() const noexcept { return {data(), size()}; }
    void reset() noexcept { setp(buf_, buf_ + MaxN); };

  private:
    char buf_[MaxN];
};

template <std::size_t MaxN>
class StaticStream : public std::ostream {
  public:
    StaticStream()
    : std::ostream{nullptr}
    {
        rdbuf(&buf_);
    }
    ~StaticStream() override = default;

    // Copy.
    StaticStream(const StaticStream& rhs) = delete;
    StaticStream& operator=(const StaticStream& rhs) = delete;

    // Move.
    StaticStream(StaticStream&&) = delete;
    StaticStream& operator=(StaticStream&&) = delete;

    const char* data() const noexcept { return buf_.data(); }
    bool empty() const noexcept { return buf_.empty(); }
    std::size_t size() const noexcept { return buf_.size(); }
    std::string_view str() const noexcept { return buf_.str(); }
    operator std::string_view() const noexcept { return buf_.str(); }
    void reset() noexcept
    {
        buf_.reset();
        swirly::reset(*this);
    };

  private:
    StaticBuf<MaxN> buf_;
};

template <std::size_t MaxN, typename ValueT>
auto& operator<<(StaticStream<MaxN>& ss, ValueT&& val)
{
    static_cast<std::ostream&>(ss) << std::forward<ValueT>(val);
    return ss;
}

using OStreamJoiner = std::experimental::ostream_joiner<char>;

template <auto DelimT, typename ArgT, typename... ArgsT>
void join(std::ostream& os, const ArgT& arg, const ArgsT&... args)
{
    os << arg;
    (..., [&os](const auto& arg) { os << DelimT << arg; }(args));
}

} // namespace util
} // namespace swirly

namespace std::experimental {
template <typename ValueT>
ostream_joiner<char>& operator<<(ostream_joiner<char>& osj, const ValueT& value)
{
    osj = value;
    return osj;
}
} // namespace std::experimental

#endif // SWIRLY_UTIL_STREAM_HPP
