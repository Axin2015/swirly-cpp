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

#include <swirly/fix/Types.hpp>

#include <swirly/app/Exception.hpp>

#include <swirly/sys/Buffer.hpp>

#include <swirly/util/Time.hpp>

#include <cassert>

namespace swirly {
using namespace std::literals::string_view_literals;
inline namespace fix {

template <typename DerivedT>
class BasicFixParser {
  public:
    BasicFixParser() noexcept = default;
    ~BasicFixParser() = default;

    // Copy.
    BasicFixParser(const BasicFixParser&) = default;
    BasicFixParser& operator=(const BasicFixParser&) = default;

    // Move.
    BasicFixParser(BasicFixParser&&) = default;
    BasicFixParser& operator=(BasicFixParser&&) = default;

    std::size_t parse(CyclTime now, ConstBuffer buf)
    {
        // If body length has not been read.
        if (msg_type_off_ == 0) {
            // Then try to read it.
            if (!parse_body_len(buf)) {
                return 0;
            }
        }
        std::size_t sum{0};
        do {
            const std::size_t len{msg_type_off_ + body_len_ + CheckSumLen};
            // Break if incomplete message.
            if (buffer_size(buf) < len) {
                break;
            }
            const auto* const begin = buffer_cast<const char*>(buf);
            const Version ver{begin[6] - '0', begin[8] - '0'};
            static_cast<DerivedT*>(this)->on_message(now, {begin, len}, msg_type_off_, ver);
            // Skip to next message.
            buf = advance(buf, len);
            sum += len;
            clear();
            // Attempt to read next message.
        } while (parse_body_len(buf));
        return sum;
    }

  private:
    void clear() noexcept
    {
        msg_type_off_ = 0;
        body_len_ = 0;
    }
    bool parse_body_len(ConstBuffer buf)
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
        std::size_t body_len{0};
        if (std::isdigit(*it)) {
            body_len = *it++ - '0';
            for (;;) {
                if (it == end) {
                    // Partial body length.
                    return 0;
                }
                if (!std::isdigit(*it)) {
                    break;
                }
                body_len *= 10;
                body_len += *it++ - '0';
            }
        }
        // Verify that first non-digit charactor was SOH.
        if (*it != Soh) {
            throw ProtocolException{"invalid FIX body length"sv};
        }
        msg_type_off_ = it + 1 - begin;
        body_len_ = body_len;
        return true;
    }
    std::size_t msg_type_off_{0}, body_len_{0};
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_PARSER_HPP
