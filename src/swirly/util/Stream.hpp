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

#include <ostream>
#include <string_view>

namespace swirly {
inline namespace util {

SWIRLY_API void reset(std::ostream& os) noexcept;

template <std::size_t MaxN>
class StringStreamBuf : public std::streambuf {
  public:
    StringStreamBuf() noexcept { reset(); }
    ~StringStreamBuf() override = default;

    // Copy.
    StringStreamBuf(const StringStreamBuf& rhs) = delete;
    StringStreamBuf& operator=(const StringStreamBuf& rhs) = delete;

    // Move.
    StringStreamBuf(StringStreamBuf&&) = delete;
    StringStreamBuf& operator=(StringStreamBuf&&) = delete;

    const char* data() const noexcept { return pbase(); }
    bool empty() const noexcept { return pbase() == pptr(); }
    std::size_t size() const noexcept { return pptr() - pbase(); }
    std::string_view str() const noexcept { return {data(), size()}; }
    void reset() noexcept { setp(buf_, buf_ + MaxN); };

  private:
    char buf_[MaxN];
};

template <std::size_t MaxN>
class StringStream : public std::ostream {
  public:
    StringStream()
    : std::ostream{nullptr}
    {
        rdbuf(&buf_);
    }
    ~StringStream() override = default;

    // Copy.
    StringStream(const StringStream& rhs) = delete;
    StringStream& operator=(const StringStream& rhs) = delete;

    // Move.
    StringStream(StringStream&&) = delete;
    StringStream& operator=(StringStream&&) = delete;

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
    StringStreamBuf<MaxN> buf_;
};

template <std::size_t MaxN, typename ValueT>
auto& operator<<(StringStream<MaxN>& ss, ValueT&& val)
{
    static_cast<std::ostream&>(ss) << std::forward<ValueT>(val);
    return ss;
}

/**
 * Stream joiner. This is a simplified version of std::experimental::ostream_joiner, intended as a
 * placeholder until TS v2 is more widely available.
 */
class SWIRLY_API OStreamJoiner {
  public:
    using char_type = char;
    using traits_type = std::char_traits<char_type>;
    using ostream_type = std::ostream;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using iterator_category = std::output_iterator_tag;

    OStreamJoiner(std::ostream& os, char sep) noexcept
    : os_{&os}
    , sep_{sep}
    {
    }
    ~OStreamJoiner();

    // Copy.
    OStreamJoiner(const OStreamJoiner&) = default;
    OStreamJoiner& operator=(const OStreamJoiner&) = default;

    // Move.
    OStreamJoiner(OStreamJoiner&&) = default;
    OStreamJoiner& operator=(OStreamJoiner&&) = default;

    template <typename ValueT>
    OStreamJoiner& operator=(const ValueT& value)
    {
        if (!first_) {
            *os_ << sep_;
        }
        first_ = false;
        *os_ << value;
        return *this;
    }
    OStreamJoiner& operator*() noexcept { return *this; }
    OStreamJoiner& operator++() noexcept { return *this; }
    OStreamJoiner& operator++(int)noexcept { return *this; }

  private:
    std::ostream* os_;
    char sep_;
    bool first_{true};
};

template <typename ValueT>
OStreamJoiner& operator<<(OStreamJoiner& osj, const ValueT& value)
{
    osj = value;
    return osj;
}

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_STREAM_HPP
