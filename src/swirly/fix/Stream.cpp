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
#include "Stream.hpp"

namespace swirly {
inline namespace fix {
using namespace std;
namespace {
// Assumption: begin-string is always FIX.x.y format.
enum { BodyLenStart = "8=FIX.x.y^9="sv.size(), CheckSumLen = "10=000^"sv.size() };

void put_check_sum(char* s, uint64_t sum) noexcept
{
    int n = sum % 256;
    s[6] = '\1';
    s[5] = '0' + (n % 10);
    n /= 10;
    s[4] = '0' + (n % 10);
    n /= 10;
    s[3] = '0' + (n % 10);
    s[2] = '=';
    s[1] = '0';
    s[0] = '1';
}
} // namespace

FixBuf::~FixBuf() = default;

void FixBuf::commit() noexcept
{
    if (pcount_ > 0) {
        put_check_sum(pbase_ + pcount_, sum_);
        buf_.commit(pcount_ + CheckSumLen);
    }
}

void FixBuf::set_body_length(std::streamsize pos, std::streamsize len) noexcept
{
    auto it = pbase_ + pos;
    do {
        const auto d = len % 10;
        sum_ += d;
        *--it = '0' + d;
        len /= 10;
    } while (len > 0);
}

FixBuf::int_type FixBuf::overflow(int_type c) noexcept
{
    if (c != traits_type::eof()) {
        auto buf = buf_.prepare(pcount_ + 1 + CheckSumLen);
        pbase_ = buffer_cast<char*>(buf);
        pbase_[pcount_++] = c;
        sum_ += c;
    }
    return c;
}

streamsize FixBuf::xsputn(const char_type* s, streamsize count) noexcept
{
    auto buf = buf_.prepare(pcount_ + count + CheckSumLen);
    pbase_ = buffer_cast<char*>(buf);
    memcpy(pbase_ + pcount_, s, count);
    pcount_ += count;
    for (int i{0}; i < count; ++i) {
        sum_ += s[i];
    }
    return count;
}

FixStream::~FixStream() = default;

void FixStream::commit() noexcept
{
    if (bloff_ > 0) {
        buf_.set_body_length(bloff_ - 1, pcount() - bloff_);
    }
    buf_.commit();
}

void FixStream::reset(Version ver)
{
    buf_.reset();
    swirly::reset(*this);
    *this << "8=FIX." << ver.major << '.' << ver.minor << "\19=0000\1";
    bloff_ = pcount();
}

} // namespace fix
} // namespace swirly
