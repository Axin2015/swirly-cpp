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
#include "UdpSocket.hpp"

namespace swirly {
inline namespace sys {

IpMcastGroup::IpMcastGroup(const IpAddress& addr, unsigned ifindex) noexcept
{
    if (addr.is_v6()) {
        const auto& bytes = addr.to_v6().to_bytes();
        family = AF_INET6;
        memcpy(&ipv6.ipv6mr_multiaddr, bytes.data(), bytes.size());
        ipv6.ipv6mr_interface = ifindex;
    } else {
        assert(addr.is_v4());
        const auto& bytes = addr.to_v4().to_bytes();
        family = AF_INET;
        memcpy(&ipv4.imr_multiaddr, bytes.data(), bytes.size());
        ipv4.imr_ifindex = ifindex;
    }
}

} // namespace sys
} // namespace swirly
