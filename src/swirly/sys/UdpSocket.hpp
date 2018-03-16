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
#ifndef SWIRLY_SYS_UDPSOCKET_HPP
#define SWIRLY_SYS_UDPSOCKET_HPP

#include <swirly/sys/IoSocket.hpp>

namespace swirly {
inline namespace sys {

struct SWIRLY_API IpMcastGroup {
    IpMcastGroup(const IpAddress& addr, unsigned ifindex = 0) noexcept;

    IpMcastGroup(const IpAddress& addr, const char* ifname, std::error_code& ec) noexcept
    : IpMcastGroup(addr, os::if_nametoindex(ifname, ec))
    {
    }
    IpMcastGroup(const IpAddress& addr, const char* ifname)
    : IpMcastGroup(addr, os::if_nametoindex(ifname))
    {
    }

    int family;
    union {
        ip_mreqn ipv4;
        ipv6_mreq ipv6;
    };
};

/**
 * Join a multicast group.
 */
inline void joinGroup(int sockfd, const IpMcastGroup& group, std::error_code& ec) noexcept
{
    if (group.family == AF_INET6) {
        os::setsockopt(sockfd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &group.ipv6, sizeof(group.ipv6), ec);
    } else {
        assert(group.family == AF_INET);
        os::setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group.ipv4, sizeof(group.ipv4), ec);
    }
}

/**
 * Join a multicast group.
 */
inline void joinGroup(int sockfd, const IpMcastGroup& group)
{
    if (group.family == AF_INET6) {
        os::setsockopt(sockfd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &group.ipv6, sizeof(group.ipv6));
    } else {
        assert(group.family == AF_INET);
        os::setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group.ipv4, sizeof(group.ipv4));
    }
}

/**
 * Join a multicast group.
 */
inline void joinGroup(int sockfd, const IpAddress& addr, unsigned ifindex,
                      std::error_code& ec) noexcept
{
    joinGroup(sockfd, IpMcastGroup{addr, ifindex}, ec);
}

/**
 * Join a multicast group.
 */
inline void joinGroup(int sockfd, const IpAddress& addr, unsigned ifindex)
{
    joinGroup(sockfd, IpMcastGroup{addr, ifindex});
}

/**
 * Join a multicast group. The system will choose an appropriate interface if ifname is null.
 */
inline void joinGroup(int sockfd, const IpAddress& addr, const char* ifname,
                      std::error_code& ec) noexcept
{
    joinGroup(sockfd, IpMcastGroup{addr, ifname}, ec);
}

/**
 * Join a multicast group. The system will choose an appropriate interface if ifname is null.
 */
inline void joinGroup(int sockfd, const IpAddress& addr, const char* ifname)
{
    joinGroup(sockfd, IpMcastGroup{addr, ifname});
}

/**
 * Leave a multicast group.
 */
inline void leaveGroup(int sockfd, const IpMcastGroup& group, std::error_code& ec) noexcept
{
    if (group.family == AF_INET6) {
        os::setsockopt(sockfd, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &group.ipv6, sizeof(group.ipv6), ec);
    } else {
        assert(group.family == AF_INET);
        os::setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &group.ipv4, sizeof(group.ipv4), ec);
    }
}

/**
 * Leave a multicast group.
 */
inline void leaveGroup(int sockfd, const IpMcastGroup& group)
{
    if (group.family == AF_INET6) {
        os::setsockopt(sockfd, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &group.ipv6, sizeof(group.ipv6));
    } else {
        assert(group.family == AF_INET);
        os::setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &group.ipv4, sizeof(group.ipv4));
    }
}

/**
 * Leave a multicast group.
 */
inline void leaveGroup(int sockfd, const IpAddress& addr, unsigned ifindex,
                       std::error_code& ec) noexcept
{
    leaveGroup(sockfd, IpMcastGroup{addr, ifindex}, ec);
}

/**
 * Leave a multicast group.
 */
inline void leaveGroup(int sockfd, const IpAddress& addr, unsigned ifindex)
{
    leaveGroup(sockfd, IpMcastGroup{addr, ifindex});
}

/**
 * Leave a multicast group. The system will leave on the first matching interface if ifname is null.
 */
inline void leaveGroup(int sockfd, const IpAddress& addr, const char* ifname,
                       std::error_code& ec) noexcept
{
    leaveGroup(sockfd, IpMcastGroup{addr, ifname}, ec);
}

/**
 * Leave a multicast group. The system will leave on the first matching interface if ifname is null.
 */
inline void leaveGroup(int sockfd, const IpAddress& addr, const char* ifname)
{
    leaveGroup(sockfd, IpMcastGroup{addr, ifname});
}

inline void setIpMcastIf(int sockfd, int family, unsigned ifindex, std::error_code& ec) noexcept
{
    if (family == AF_INET6) {
        os::setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifindex, sizeof(ifindex), ec);
    } else {
        assert(family == AF_INET);
        ip_mreqn mreqn{};
        mreqn.imr_ifindex = ifindex;
        os::setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &mreqn, sizeof(mreqn), ec);
    }
}

inline void setIpMcastIf(int sockfd, int family, unsigned ifindex)
{
    if (family == AF_INET6) {
        os::setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifindex, sizeof(ifindex));
    } else {
        assert(family == AF_INET);
        ip_mreqn mreqn{};
        mreqn.imr_ifindex = ifindex;
        os::setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &mreqn, sizeof(mreqn));
    }
}

inline void setIpMcastIf(int sockfd, int family, const char* ifname, std::error_code& ec) noexcept
{
    setIpMcastIf(sockfd, family, os::if_nametoindex(ifname), ec);
}

inline void setIpMcastIf(int sockfd, int family, const char* ifname)
{
    setIpMcastIf(sockfd, family, os::if_nametoindex(ifname));
}

/**
 * Determines whether sent multicast packets should be looped back to the local sockets.
 */
inline void setIpMcastLoop(int sockfd, int family, bool enabled, std::error_code& ec) noexcept
{
    if (family == AF_INET6) {
        const unsigned optval{enabled ? 1U : 0U};
        os::setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &optval, sizeof(optval), ec);
    } else {
        assert(family == AF_INET);
        const unsigned char optval = enabled ? 1 : 0;
        os::setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &optval, sizeof(optval), ec);
    }
}

/**
 * Determines whether sent multicast packets should be looped back to the local sockets.
 */
inline void setIpMcastLoop(int sockfd, int family, bool enabled)
{
    if (family == AF_INET6) {
        const unsigned optval{enabled ? 1U : 0U};
        os::setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &optval, sizeof(optval));
    } else {
        assert(family == AF_INET);
        const unsigned char optval = enabled ? 1 : 0;
        os::setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &optval, sizeof(optval));
    }
}

/**
 * Set or read the time-to-live value of outgoing multicast packets for this socket.
 */
inline void setIpMcastTtl(int sockfd, int family, int ttl, std::error_code& ec) noexcept
{
    if (family == AF_INET6) {
        const int optval{ttl};
        os::setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &optval, sizeof(optval), ec);
    } else {
        assert(family == AF_INET);
        const unsigned char optval = ttl;
        os::setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &optval, sizeof(optval), ec);
    }
}

/**
 * Set or read the time-to-live value of outgoing multicast packets for this socket.
 */
inline void setIpMcastTtl(int sockfd, int family, int ttl)
{
    if (family == AF_INET6) {
        const int optval{ttl};
        os::setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &optval, sizeof(optval));
    } else {
        assert(family == AF_INET);
        const unsigned char optval = ttl;
        os::setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &optval, sizeof(optval));
    }
}

/**
 * Connectionless UDP Socket. All state is in base class, so object can be sliced.
 */
struct UdpSocket : IoSocket {
    using Transport = Udp;
    using Endpoint = UdpEndpoint;

    using IoSocket::IoSocket;
    UdpSocket(Transport trans, std::error_code& ec) noexcept
    : IoSocket{os::socket(trans, ec), trans.family()}
    {
    }
    explicit UdpSocket(Transport trans)
    : IoSocket{os::socket(trans), trans.family()}
    {
    }
    UdpSocket() noexcept = default;

    void bind(const Endpoint& ep, std::error_code& ec) noexcept { os::bind(*sock_, ep, ec); }
    void bind(const Endpoint& ep) { os::bind(*sock_, ep); }

    ssize_t recvfrom(void* buf, std::size_t len, int flags, Endpoint& ep,
                     std::error_code& ec) noexcept
    {
        return os::recvfrom(*sock_, buf, len, flags, ep, ec);
    }
    std::size_t recvfrom(void* buf, std::size_t len, int flags, Endpoint& ep)
    {
        return os::recvfrom(*sock_, buf, len, flags, ep);
    }

    ssize_t recvfrom(const MutableBuffer& buf, int flags, Endpoint& ep,
                     std::error_code& ec) noexcept
    {
        return os::recvfrom(*sock_, buf, flags, ep, ec);
    }
    std::size_t recvfrom(const MutableBuffer& buf, int flags, Endpoint& ep)
    {
        return os::recvfrom(*sock_, buf, flags, ep);
    }

    ssize_t sendto(const void* buf, std::size_t len, int flags, const Endpoint& ep,
                   std::error_code& ec) noexcept
    {
        return os::sendto(*sock_, buf, len, flags, ep, ec);
    }
    std::size_t sendto(const void* buf, std::size_t len, int flags, const Endpoint& ep)
    {
        return os::sendto(*sock_, buf, len, flags, ep);
    }

    ssize_t sendto(const ConstBuffer& buf, int flags, const Endpoint& ep,
                   std::error_code& ec) noexcept
    {
        return os::sendto(*sock_, buf, flags, ep, ec);
    }
    std::size_t sendto(const ConstBuffer& buf, int flags, const Endpoint& ep)
    {
        return os::sendto(*sock_, buf, flags, ep);
    }

    void joinGroup(const IpAddress& addr, unsigned ifindex, std::error_code& ec) noexcept
    {
        return swirly::joinGroup(*sock_, addr, ifindex, ec);
    }
    void joinGroup(const IpAddress& addr, unsigned ifindex)
    {
        return swirly::joinGroup(*sock_, addr, ifindex);
    }

    void joinGroup(const IpAddress& addr, const char* ifname, std::error_code& ec) noexcept
    {
        return swirly::joinGroup(*sock_, addr, ifname, ec);
    }
    void joinGroup(const IpAddress& addr, const char* ifname)
    {
        return swirly::joinGroup(*sock_, addr, ifname);
    }

    void leaveGroup(const IpAddress& addr, unsigned ifindex, std::error_code& ec) noexcept
    {
        return swirly::leaveGroup(*sock_, addr, ifindex, ec);
    }
    void leaveGroup(const IpAddress& addr, unsigned ifindex)
    {
        return swirly::leaveGroup(*sock_, addr, ifindex);
    }

    void leaveGroup(const IpAddress& addr, const char* ifname, std::error_code& ec) noexcept
    {
        return swirly::leaveGroup(*sock_, addr, ifname, ec);
    }
    void leaveGroup(const IpAddress& addr, const char* ifname)
    {
        return swirly::leaveGroup(*sock_, addr, ifname);
    }

    void setIpMcastIf(const char* ifname, std::error_code& ec) noexcept
    {
        return swirly::setIpMcastIf(*sock_, family_, ifname, ec);
    }
    void setIpMcastIf(const char* ifname) { return swirly::setIpMcastIf(*sock_, family_, ifname); }

    void setIpMcastLoop(bool enabled, std::error_code& ec) noexcept
    {
        return swirly::setIpMcastLoop(*sock_, family_, enabled, ec);
    }
    void setIpMcastLoop(bool enabled) { return swirly::setIpMcastLoop(*sock_, family_, enabled); }

    void setIpMcastTtl(int ttl, std::error_code& ec) noexcept
    {
        return swirly::setIpMcastTtl(*sock_, family_, ttl, ec);
    }
    void setIpMcastTtl(int ttl) { return swirly::setIpMcastTtl(*sock_, family_, ttl); }
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_UDPSOCKET_HPP
