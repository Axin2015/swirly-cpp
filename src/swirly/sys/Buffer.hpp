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
#ifndef SWIRLY_SYS_BUFFER_HPP
#define SWIRLY_SYS_BUFFER_HPP

#include <swirly/Config.h>

#include <boost/asio/buffer.hpp>

namespace swirly {
inline namespace sys {

using ConstBuffer = boost::asio::const_buffer;
using MutableBuffer = boost::asio::mutable_buffer;

using boost::asio::buffer_cast;
using boost::asio::buffer_size;

class SWIRLY_API Buffer {
  public:
    explicit Buffer(std::size_t capacity) { buf_.reserve(capacity); }
    Buffer() = default;
    ~Buffer() = default;

    // Copy.
    Buffer(const Buffer& rhs) = default;
    Buffer& operator=(const Buffer& rhs) = default;

    // Move.
    Buffer(Buffer&& rhs) noexcept = default;
    Buffer& operator=(Buffer&& rhs) noexcept = default;

    /**
     * Returns available data as a buffer.
     */
    ConstBuffer data() const noexcept { return {rptr(), size()}; }

    /**
     * Returns slice of available data as a buffer.
     */
    ConstBuffer data(std::size_t limit) const noexcept { return {rptr(), std::min(limit, size())}; }

    /**
     * Returns available data as a string view.
     */
    std::string_view str() const noexcept { return {rptr(), size()}; }

    /**
     * Returns slice of available data as a string view.
     */
    std::string_view str(std::size_t limit) const noexcept
    {
        return {rptr(), std::min(limit, size())};
    }

    /**
     * Returns true if read buffer is empty.
     */
    bool empty() const noexcept { return size() == 0U; };

    /**
     * Returns number of bytes available for read.
     */
    std::size_t size() const noexcept { return wpos_ - rpos_; }

    /**
     * Clear buffer.
     */
    void clear() noexcept
    {
        rpos_ = wpos_ = 0;
        buf_.clear();
    }

    /**
     * Move characters from the write sequence to the read sequence.
     */
    void commit(std::size_t count) noexcept { wpos_ += count; }

    /**
     * Remove characters from the read sequence.
     */
    void consume(std::size_t count) noexcept;

    /**
     * Returns write buffer of specified size.
     */
    MutableBuffer prepare(std::size_t size);

    /**
     * Reserve storage.
     */
    void reserve(std::size_t capacity) { buf_.reserve(capacity); }

  private:
    const char* rptr() const noexcept { return buf_.data() + rpos_; }
    char* wptr() noexcept { return buf_.data() + wpos_; }
    std::size_t unused() const noexcept { return buf_.size() - wpos_; }

    std::size_t rpos_{}, wpos_{};
    std::vector<char> buf_;
};

SWIRLY_API ConstBuffer advance(ConstBuffer buf, std::size_t n) noexcept;

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_BUFFER_HPP
