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
#ifndef SWIRLY_SYS_IPADDRESS_HPP
#define SWIRLY_SYS_IPADDRESS_HPP

#include <swirly/util/TypeTraits.hpp>

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

namespace swirly {
inline namespace sys {

using IpAddress = boost::asio::ip::address;
using IpAddressV4 = boost::asio::ip::address_v4;
using IpAddressV6 = boost::asio::ip::address_v6;

using Tcp = boost::asio::ip::tcp;
using Udp = boost::asio::ip::udp;

template <typename TransportT>
using BasicEndpoint = boost::asio::ip::basic_endpoint<TransportT>;

using TcpEndpoint = BasicEndpoint<Tcp>;
using UdpEndpoint = BasicEndpoint<Udp>;

template <typename TransportT>
struct SWIRLY_API TransportTraits;

template <>
struct SWIRLY_API TransportTraits<Tcp> {
    static TcpEndpoint parse_endpoint(const std::string& s, boost::system::error_code& ec);
    static TcpEndpoint parse_endpoint(const std::string& s);
};

template <>
struct SWIRLY_API TransportTraits<Udp> {
    static UdpEndpoint parse_endpoint(const std::string& s, boost::system::error_code& ec);
    static UdpEndpoint parse_endpoint(const std::string& s);
};

template <typename TransportT>
BasicEndpoint<TransportT> parse_endpoint(const std::string& s, boost::system::error_code& ec)
{
    return TransportTraits<TransportT>::parse_endpoint(s, ec);
}

template <typename TransportT>
BasicEndpoint<TransportT> parse_endpoint(const std::string& s)
{
    return TransportTraits<TransportT>::parse_endpoint(s);
}

} // namespace sys
} // namespace swirly

namespace swirly {
inline namespace util {
template <typename TransportT>
struct TypeTraits<sys::BasicEndpoint<TransportT>> {
    static auto from_string(std::string_view sv) { return sys::parse_endpoint<TransportT>(sv); }
    static auto from_string(const std::string& s) { return sys::parse_endpoint<TransportT>(s); }
};
} // namespace util
} // namespace swirly

namespace boost {
namespace asio {
namespace ip {
SWIRLY_API std::istream& operator>>(std::istream& is, basic_endpoint<tcp>& ep);

SWIRLY_API std::istream& operator>>(std::istream& is, basic_endpoint<udp>& ep);
} // namespace ip
} // namespace asio
} // namespace boost

#endif // SWIRLY_SYS_IPADDRESS_HPP
