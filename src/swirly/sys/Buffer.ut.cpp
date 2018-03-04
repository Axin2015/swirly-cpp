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

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

namespace {

void write(Buffer& buf, const char* data)
{
    const auto len = strlen(data);
    const auto out = buf.prepare(len);
    memcpy(buffer_cast<char*>(out), data, len);
    buf.commit(len);
}

string read(Buffer& buf)
{
    const auto in = buf.data();
    string s{buffer_cast<const char*>(in), buffer_size(in)};
    buf.consume(buffer_size(in));
    return s;
}

string read(Buffer& buf, std::size_t limit)
{
    const auto in = buf.data(limit);
    string s{buffer_cast<const char*>(in), buffer_size(in)};
    buf.consume(buffer_size(in));
    return s;
}

} // namespace

SWIRLY_TEST_CASE(BufferReadWrite)
{
    Buffer buf;
    SWIRLY_CHECK(buf.empty());
    SWIRLY_CHECK(buf.size() == 0);
    SWIRLY_CHECK(buffer_size(buf.data()) == 0);

    write(buf, "foo");
    SWIRLY_CHECK(!buf.empty());
    SWIRLY_CHECK(buf.size() == 3);
    SWIRLY_CHECK(buffer_size(buf.data()) == 3);
    SWIRLY_CHECK(memcmp(buffer_cast<const char*>(buf.data()), "foo", 3) == 0);

    write(buf, "bar");
    SWIRLY_CHECK(!buf.empty());
    SWIRLY_CHECK(buf.size() == 6);
    SWIRLY_CHECK(buffer_size(buf.data()) == 6);
    SWIRLY_CHECK(memcmp(buffer_cast<const char*>(buf.data()), "foobar", 6) == 0);

    SWIRLY_CHECK(read(buf, 4) == "foob");
    SWIRLY_CHECK(!buf.empty());
    SWIRLY_CHECK(buf.size() == 2);
    SWIRLY_CHECK(buffer_size(buf.data()) == 2);
    SWIRLY_CHECK(memcmp(buffer_cast<const char*>(buf.data()), "ar", 2) == 0);

    write(buf, "baz");
    SWIRLY_CHECK(!buf.empty());
    SWIRLY_CHECK(buf.size() == 5);
    SWIRLY_CHECK(buffer_size(buf.data()) == 5);
    SWIRLY_CHECK(memcmp(buffer_cast<const char*>(buf.data()), "arbaz", 5) == 0);

    SWIRLY_CHECK(read(buf) == "arbaz");
    SWIRLY_CHECK(buf.empty());
    SWIRLY_CHECK(buf.size() == 0);
    SWIRLY_CHECK(buffer_size(buf.data()) == 0);
}

SWIRLY_TEST_CASE(BufferNoShrinkSmall)
{
    Buffer buf;

    // Avoid vector reallocation.
    buf.reserve(4096);
    // Base address.
    const auto* base = buffer_cast<const char*>(buf.data());

    // Simulated write.
    buf.prepare(16);
    buf.commit(16);
    SWIRLY_CHECK(!buf.empty());
    SWIRLY_CHECK(buf.size() == 16);
    SWIRLY_CHECK(buffer_size(buf.data()) == 16);
    SWIRLY_CHECK(buffer_cast<const char*>(buf.data()) == base);

    // Consume less than half.
    buf.consume(7);
    SWIRLY_CHECK(!buf.empty());
    SWIRLY_CHECK(buf.size() == 9);
    SWIRLY_CHECK(buffer_size(buf.data()) == 9);
    SWIRLY_CHECK(buffer_cast<const char*>(buf.data()) == base + 7);

    // Consuming one more should not trigger shrink, because the buffer size is less than the shrink
    // threshold.
    buf.consume(1);
    SWIRLY_CHECK(!buf.empty());
    SWIRLY_CHECK(buf.size() == 8);
    SWIRLY_CHECK(buffer_size(buf.data()) == 8);
    SWIRLY_CHECK(buffer_cast<const char*>(buf.data()) != base);
}

SWIRLY_TEST_CASE(BufferShrinkLarge)
{
    Buffer buf;

    // Avoid vector reallocation.
    buf.reserve(4096);
    // Base address.
    const auto* base = buffer_cast<const char*>(buf.data());

    // Simulated write.
    buf.prepare(2048);
    buf.commit(2048);
    SWIRLY_CHECK(!buf.empty());
    SWIRLY_CHECK(buf.size() == 2048);
    SWIRLY_CHECK(buffer_size(buf.data()) == 2048);
    SWIRLY_CHECK(buffer_cast<const char*>(buf.data()) == base);

    // Consume less than half.
    buf.consume(1023);
    SWIRLY_CHECK(!buf.empty());
    SWIRLY_CHECK(buf.size() == 1025);
    SWIRLY_CHECK(buffer_size(buf.data()) == 1025);
    SWIRLY_CHECK(buffer_cast<const char*>(buf.data()) == base + 1023);

    // Consuming one more should not trigger shrink, because the buffer size is less than the shrink
    // threshold.
    buf.consume(1);
    SWIRLY_CHECK(!buf.empty());
    SWIRLY_CHECK(buf.size() == 1024);
    SWIRLY_CHECK(buffer_size(buf.data()) == 1024);
    SWIRLY_CHECK(buffer_cast<const char*>(buf.data()) == base);
}
