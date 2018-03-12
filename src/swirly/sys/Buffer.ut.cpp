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

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_SUITE(BufferSuite)

BOOST_AUTO_TEST_CASE(ReadWriteCase)
{
    Buffer buf;
    BOOST_TEST(buf.empty());
    BOOST_TEST(buf.size() == 0);
    BOOST_TEST(buffer_size(buf.data()) == 0);

    write(buf, "foo");
    BOOST_TEST(!buf.empty());
    BOOST_TEST(buf.size() == 3);
    BOOST_TEST(buffer_size(buf.data()) == 3);
    BOOST_TEST(memcmp(buffer_cast<const char*>(buf.data()), "foo", 3) == 0);

    write(buf, "bar");
    BOOST_TEST(!buf.empty());
    BOOST_TEST(buf.size() == 6);
    BOOST_TEST(buffer_size(buf.data()) == 6);
    BOOST_TEST(memcmp(buffer_cast<const char*>(buf.data()), "foobar", 6) == 0);

    BOOST_TEST(read(buf, 4) == "foob");
    BOOST_TEST(!buf.empty());
    BOOST_TEST(buf.size() == 2);
    BOOST_TEST(buffer_size(buf.data()) == 2);
    BOOST_TEST(memcmp(buffer_cast<const char*>(buf.data()), "ar", 2) == 0);

    write(buf, "baz");
    BOOST_TEST(!buf.empty());
    BOOST_TEST(buf.size() == 5);
    BOOST_TEST(buffer_size(buf.data()) == 5);
    BOOST_TEST(memcmp(buffer_cast<const char*>(buf.data()), "arbaz", 5) == 0);

    BOOST_TEST(read(buf) == "arbaz");
    BOOST_TEST(buf.empty());
    BOOST_TEST(buf.size() == 0);
    BOOST_TEST(buffer_size(buf.data()) == 0);
}

BOOST_AUTO_TEST_CASE(NoShrinkSmallCase)
{
    Buffer buf;

    // Avoid vector reallocation.
    buf.reserve(4096);
    // Base address.
    const auto* base = buffer_cast<const char*>(buf.data());

    // Simulated write.
    buf.prepare(16);
    buf.commit(16);
    BOOST_TEST(!buf.empty());
    BOOST_TEST(buf.size() == 16);
    BOOST_TEST(buffer_size(buf.data()) == 16);
    BOOST_TEST(distance(base, buffer_cast<const char*>(buf.data())) == 0);

    // Consume less than half.
    buf.consume(7);
    BOOST_TEST(!buf.empty());
    BOOST_TEST(buf.size() == 9);
    BOOST_TEST(buffer_size(buf.data()) == 9);
    BOOST_TEST(distance(base, buffer_cast<const char*>(buf.data())) == 7);

    // Consuming one more should not trigger shrink, because the buffer size is less than the shrink
    // threshold.
    buf.consume(1);
    BOOST_TEST(!buf.empty());
    BOOST_TEST(buf.size() == 8);
    BOOST_TEST(buffer_size(buf.data()) == 8);
    BOOST_TEST(distance(base, buffer_cast<const char*>(buf.data())) != 0);
}

BOOST_AUTO_TEST_CASE(ShrinkLargeCase)
{
    Buffer buf;

    // Avoid vector reallocation.
    buf.reserve(4096);
    // Base address.
    const auto* base = buffer_cast<const char*>(buf.data());

    // Simulated write.
    buf.prepare(2048);
    buf.commit(2048);
    BOOST_TEST(!buf.empty());
    BOOST_TEST(buf.size() == 2048);
    BOOST_TEST(buffer_size(buf.data()) == 2048);
    BOOST_TEST(buffer_cast<const char*>(buf.data()) == base);

    // Consume less than half.
    buf.consume(1023);
    BOOST_TEST(!buf.empty());
    BOOST_TEST(buf.size() == 1025);
    BOOST_TEST(buffer_size(buf.data()) == 1025);
    BOOST_TEST(buffer_cast<const char*>(buf.data()) == base + 1023);

    // Consuming one more should not trigger shrink, because the buffer size is less than the shrink
    // threshold.
    buf.consume(1);
    BOOST_TEST(!buf.empty());
    BOOST_TEST(buf.size() == 1024);
    BOOST_TEST(buffer_size(buf.data()) == 1024);
    BOOST_TEST(buffer_cast<const char*>(buf.data()) == base);
}

BOOST_AUTO_TEST_SUITE_END()
