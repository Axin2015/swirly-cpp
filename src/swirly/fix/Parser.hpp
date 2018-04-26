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
#ifndef SWIRLY_FIX_PARSER_HPP
#define SWIRLY_FIX_PARSER_HPP

#include <swirly/fix/Exception.hpp>
#include <swirly/fix/Types.hpp>

#include <swirly/sys/Buffer.hpp>

#include <cassert>

namespace swirly {
using namespace std::literals::string_view_literals;
inline namespace fix {

template <typename DerivedT>
class BasicFixParser {
    // Assumption: begin-string is always FIX.x.y format.
    enum { BodyLenStart = "8=FIX.x.y^9="sv.size(), CheckSumLen = "10=000^"sv.size() };
    // Ascii 1: Start Of Header.
    static constexpr auto Soh = '\01';

  public:
    BasicFixParser() noexcept = default;
    ~BasicFixParser() = default;

    // Copy.
    BasicFixParser(const BasicFixParser&) = default;
    BasicFixParser& operator=(const BasicFixParser&) = default;

    // Move.
    BasicFixParser(BasicFixParser&&) = default;
    BasicFixParser& operator=(BasicFixParser&&) = default;

    std::size_t parse(ConstBuffer buf)
    {
        // If body length has not been read.
        if (offset_ == 0) {
            // Then try to read it.
            if (!parseBodyLen(buf)) {
                return 0;
            }
        }
        std::size_t sum{0};
        do {
            const std::size_t len{offset_ + bodyLen_ + CheckSumLen};
            // Break if incomplete message.
            if (buffer_size(buf) < len) {
                break;
            }
            const auto* const begin = buffer_cast<const char*>(buf);
            const FixVersion ver{begin[6] - '0', begin[8] - '0'};
            static_cast<DerivedT*>(this)->onMessage(ver, {begin + offset_, bodyLen_});
            // Skip to next message.
            buf = advance(buf, len);
            sum += len;
            clear();
            // Attempt to read next message.
        } while (parseBodyLen(buf));
        return sum;
    }

  private:
    void clear() noexcept
    {
        offset_ = 0;
        bodyLen_ = 0;
    }
    bool parseBodyLen(ConstBuffer buf)
    {
        const auto len = buffer_size(buf);
        if (len <= BodyLenStart) {
            return false;
        }
        const auto* const begin = buffer_cast<const char*>(buf);
        const auto* const end = begin + len;

        auto* it = begin + BodyLenStart;
        assert(it != end);

        // Attempt to extract body length digits.
        std::size_t bodyLen{0};
        if (std::isdigit(*it)) {
            bodyLen += *it++ - '0';
            for (;;) {
                if (it == end) {
                    // Partial body length.
                    return 0;
                }
                if (!std::isdigit(*it)) {
                    break;
                }
                bodyLen *= 10;
                bodyLen += *it++ - '0';
            }
        }
        // Verify that first non-digit charactor was SOH.
        if (*it != Soh) {
            throw FixException{"invalid FIX body length"sv};
        }
        offset_ = it + 1 - begin;
        bodyLen_ = bodyLen;
        return true;
    }
    std::size_t offset_{0}, bodyLen_{0};
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_PARSER_HPP
