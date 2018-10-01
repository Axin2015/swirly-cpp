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
#include <swirly/sys/Buffer.hpp>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

namespace swirly {
inline namespace sys {
bool operator==(const Buffer& buf, std::string_view sv)
{
    const auto data = buf.data();
    return buffer_size(data) == sv.size() //
        && memcmp(buffer_cast<const char*>(data), sv.data(), sv.size()) == 0;
}
bool operator!=(const Buffer& buf, std::string_view sv)
{
    return !(buf == sv);
}
bool operator==(std::string_view sv, const Buffer& buf)
{
    return buf == sv;
}
bool operator!=(std::string_view sv, const Buffer& buf)
{
    return buf != sv;
}
std::ostream& operator<<(std::ostream& os, const Buffer& buf)
{
    const auto data = buf.data();
    return os.write(buffer_cast<const char*>(data), buffer_size(data));
}
} // namespace sys
} // namespace swirly
