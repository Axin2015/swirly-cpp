/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#include "IpAddress.hpp"

namespace swirly {
namespace {

std::pair<std::string, int> splitPair(const std::string& s, char delim)
{
    const auto pos = s.find_last_of(delim);
    std::string addr, num;
    if (pos == std::string::npos) {
        addr = s;
    } else {
        addr = s.substr(0, pos);
        num = s.substr(pos + 1);
    }
    return {addr, num.empty() ? 0 : stoi(num)};
}

template <typename TransportT>
BasicEndpoint<TransportT> parseImpl(const std::string& s, boost::system::error_code& ec)
{
    using Endpoint = BasicEndpoint<TransportT>;

    std::string addr;
    int port;
    tie(addr, port) = splitPair(s, ':');

    Endpoint ep{};
    if (addr.size() >= 2 && addr.front() == '[' && addr.back() == ']') {
        // Strip square brackets.
        addr = addr.substr(1, addr.size() - 2);
        const auto addrv6 = IpAddressV6::from_string(addr, ec);
        ep = Endpoint{addrv6, static_cast<uint16_t>(port)};
    } else {
        const auto addrv4 = IpAddressV4::from_string(addr, ec);
        ep = Endpoint{addrv4, static_cast<uint16_t>(port)};
    }
    return ep;
}

template <typename TransportT>
BasicEndpoint<TransportT> parseImpl(const std::string& s)
{
    boost::system::error_code ec;
    const auto ep = parseEndpoint<TransportT>(s, ec);
    boost::asio::detail::throw_error(ec);
    return ep;
}

template <typename TransportT>
std::istream& parseImpl(std::istream& is, BasicEndpoint<TransportT>& ep)
{
    std::string tok;
    is >> tok;

    boost::system::error_code ec;
    ep = swirly::parseImpl<TransportT>(tok, ec);
    if (ec) {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}
} // namespace

TcpEndpoint TransportTraits<Tcp>::parseEndpoint(const std::string& s, boost::system::error_code& ec)
{
    return parseImpl<Tcp>(s, ec);
}

TcpEndpoint TransportTraits<Tcp>::parseEndpoint(const std::string& s)
{
    return parseImpl<Tcp>(s);
}

UdpEndpoint TransportTraits<Udp>::parseEndpoint(const std::string& s, boost::system::error_code& ec)
{
    return parseImpl<Udp>(s, ec);
}

UdpEndpoint TransportTraits<Udp>::parseEndpoint(const std::string& s)
{
    return parseImpl<Udp>(s);
}
} // namespace swirly

std::istream& boost::asio::ip::operator>>(std::istream& is, basic_endpoint<tcp>& ep)
{
    return swirly::parseImpl<tcp>(is, ep);
}

std::istream& boost::asio::ip::operator>>(std::istream& is, basic_endpoint<udp>& ep)
{
    return swirly::parseImpl<udp>(is, ep);
}
