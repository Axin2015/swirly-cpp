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
#include "Buffer.hpp"

namespace swirly {

void Buffer::consume(std::size_t count) noexcept
{
    enum { ShrinkThreshold = 1024 };

    rpos_ += count;

    // Shrink if the block of unused bytes at the front of the buffer satisfies the following
    // requirements: 1) greater than or equal to ShrinkThreshold, 1) greater than or equal to half
    // the total buffer size.
    if (rpos_ >= ShrinkThreshold && rpos_ >= (buf_.size() >> 1)) {

        // Then move unread portion to front.
        const auto n = size();
        if (n > 0) {
            // Move data to front.
            std::memmove(buf_.data(), rptr(), n);
        }

        rpos_ = 0;
        wpos_ = n;
    }
}

/**
     * Returns write buffer of specified size.
     */
MutableBuffer Buffer::prepare(std::size_t size)
{
    const auto n = unused();
    if (size > n) {
        // More buffer space required.
        const auto d = size - n;
        buf_.resize(buf_.size() + d);
    }
    return {wptr(), size};
}

} // namespace swirly
