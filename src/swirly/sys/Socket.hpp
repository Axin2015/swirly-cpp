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
#ifndef SWIRLY_SYS_SOCKET_HPP
#define SWIRLY_SYS_SOCKET_HPP

#include <swirly/sys/File.hpp>
#include <swirly/sys/IpAddress.hpp>

#include <sys/socket.h>

namespace swirly {

namespace sys {

/**
 * Returns the index of the network interface corresponding to the name ifname.
 */
inline unsigned if_nametoindex(const char* ifname, std::error_code& ec) noexcept
{
    unsigned ifindex{0};
    if (ifname) {
        if (!(ifindex = ::if_nametoindex(ifname))) {
            ec = makeError(errno);
        }
    }
    return ifindex;
}

/**
 * Returns the index of the network interface corresponding to the name ifname.
 */
inline unsigned if_nametoindex(const char* ifname)
{
    unsigned ifindex{0};
    if (ifname) {
        if (!(ifindex = ::if_nametoindex(ifname))) {
            throw std::system_error{makeError(errno), "if_nametoindex"};
        }
    }
    return ifindex;
}

/**
 * Create an endpoint for communication.
 */
inline File socket(int family, int type, int protocol, std::error_code& ec) noexcept
{
    const auto sockfd = ::socket(family, type, protocol);
    if (sockfd < 0) {
        ec = makeError(errno);
    }
    return sockfd;
}

/**
 * Create an endpoint for communication.
 */
inline File socket(int family, int type, int protocol)
{
    const auto sockfd = ::socket(family, type, protocol);
    if (sockfd < 0) {
        throw std::system_error{makeError(errno), "socket"};
    }
    return sockfd;
}

/**
 * Create an endpoint for communication.
 */
template <typename TransportT>
inline File socket(TransportT trans, std::error_code& ec) noexcept
{
    return socket(trans.family(), trans.type(), trans.protocol(), ec);
}

/**
 * Create an endpoint for communication.
 */
template <typename TransportT>
inline File socket(TransportT trans)
{
    return socket(trans.family(), trans.type(), trans.protocol());
}

/**
 * Create a pair of connected sockets.
 */
inline std::pair<File, File> socketpair(int family, int type, int protocol,
                                        std::error_code& ec) noexcept
{
    int sv[2];
    if (::socketpair(family, type, protocol, sv) < 0) {
        ec = makeError(errno);
    }
    return {File{sv[0]}, File{sv[1]}};
}

/**
 * Create a pair of connected sockets.
 */
inline std::pair<File, File> socketpair(int family, int type, int protocol)
{
    int sv[2];
    if (::socketpair(family, type, protocol, sv) < 0) {
        throw std::system_error{makeError(errno), "socketpair"};
    }
    return {File{sv[0]}, File{sv[1]}};
}

/**
 * Create a pair of connected sockets.
 */
template <typename TransportT>
inline std::pair<File, File> socketpair(TransportT trans, std::error_code& ec) noexcept
{
    return socketpair(trans.family(), trans.type(), trans.protocol(), ec);
}

/**
 * Create a pair of connected sockets.
 */
template <typename TransportT>
inline std::pair<File, File> socketpair(TransportT trans)
{
    return socketpair(trans.family(), trans.type(), trans.protocol());
}

/**
 * Accept a connection on a socket.
 */
inline File accept(int sockfd, sockaddr& addr, socklen_t& addrlen, std::error_code& ec) noexcept
{
    // The addrlen argument is updated to contain the actual size of the source address.
    // The returned address is truncated if the buffer provided is too small.
    const auto fd = ::accept(sockfd, &addr, &addrlen);
    if (fd < 0) {
        ec = makeError(errno);
    }
    return fd;
}

/**
 * Accept a connection on a socket.
 */
inline File accept(int sockfd, sockaddr& addr, socklen_t& addrlen)
{
    // The addrlen argument is updated to contain the actual size of the source address.
    // The returned address is truncated if the buffer provided is too small.
    const auto fd = ::accept(sockfd, &addr, &addrlen);
    if (fd < 0) {
        throw std::system_error{makeError(errno), "accept"};
    }
    return fd;
}

/**
 * Accept a connection on a socket.
 */
inline File accept(int sockfd, std::error_code& ec) noexcept
{
    const auto fd = ::accept(sockfd, nullptr, nullptr);
    if (fd < 0) {
        ec = makeError(errno);
    }
    return fd;
}

/**
 * Accept a connection on a socket.
 */
inline File accept(int sockfd)
{
    const auto fd = ::accept(sockfd, nullptr, nullptr);
    if (fd < 0) {
        throw std::system_error{makeError(errno), "accept"};
    }
    return fd;
}

/**
 * Accept a connection on a socket.
 */
template <typename TransportT>
inline File accept(int sockfd, BasicEndpoint<TransportT>& ep, std::error_code& ec) noexcept
{
    socklen_t addrlen = ep.capacity();
    File fh{accept(sockfd, *ep.data(), addrlen, ec)};
    if (fh) {
        ep.resize(std::min<std::size_t>(addrlen, ep.capacity()));
    }
    return fh;
}

/**
 * Accept a connection on a socket.
 */
template <typename TransportT>
inline File accept(int sockfd, BasicEndpoint<TransportT>& ep)
{
    socklen_t addrlen = ep.capacity();
    File fh{accept(sockfd, *ep.data(), addrlen)};
    ep.resize(std::min<std::size_t>(addrlen, ep.capacity()));
    return fh;
}

/**
 * Bind a name to a socket.
 */
inline void bind(int sockfd, const sockaddr& addr, socklen_t addrlen, std::error_code& ec) noexcept
{
    if (::bind(sockfd, &addr, addrlen) < 0) {
        ec = makeError(errno);
    }
}

/**
 * Bind a name to a socket.
 */
inline void bind(int sockfd, const sockaddr& addr, socklen_t addrlen)
{
    if (::bind(sockfd, &addr, addrlen) < 0) {
        throw std::system_error{makeError(errno), "bind"};
    }
}

/**
 * Bind a name to a socket.
 */
template <typename TransportT>
inline void bind(int sockfd, const BasicEndpoint<TransportT>& ep, std::error_code& ec) noexcept
{
    bind(sockfd, *ep.data(), ep.size(), ec);
}

/**
 * Bind a name to a socket.
 */
template <typename TransportT>
inline void bind(int sockfd, const BasicEndpoint<TransportT>& ep)
{
    bind(sockfd, *ep.data(), ep.size());
}

/**
 * Initiate a connection on a socket.
 */
inline void connect(int sockfd, const sockaddr& addr, socklen_t addrlen,
                    std::error_code& ec) noexcept
{
    if (::connect(sockfd, &addr, addrlen) < 0) {
        ec = makeError(errno);
    }
}

/**
 * Initiate a connection on a socket.
 */
inline void connect(int sockfd, const sockaddr& addr, socklen_t addrlen)
{
    if (::connect(sockfd, &addr, addrlen) < 0) {
        throw std::system_error{makeError(errno), "connect"};
    }
}

/**
 * Initiate a connection on a socket.
 */
template <typename TransportT>
inline void connect(int sockfd, const BasicEndpoint<TransportT>& ep, std::error_code& ec) noexcept
{
    connect(sockfd, *ep.data(), ep.size(), ec);
}

/**
 * Initiate a connection on a socket.
 */
template <typename TransportT>
inline void connect(int sockfd, const BasicEndpoint<TransportT>& ep)
{
    connect(sockfd, *ep.data(), ep.size());
}

/**
 * Listen for connections on a socket.
 */
inline void listen(int sockfd, int backlog, std::error_code& ec) noexcept
{
    if (::listen(sockfd, backlog) < 0) {
        ec = makeError(errno);
    }
}

/**
 * Listen for connections on a socket.
 */
inline void listen(int sockfd, int backlog)
{
    if (::listen(sockfd, backlog) < 0) {
        throw std::system_error{makeError(errno), "listen"};
    }
}

/**
 * Shut-down part of a full-duplex connection
 */
inline void shutdown(int sockfd, int how, std::error_code& ec) noexcept
{
    if (::shutdown(sockfd, how) < 0) {
        ec = makeError(errno);
    }
}

/**
 * Shut-down part of a full-duplex connection
 */
inline void shutdown(int sockfd, int how)
{
    if (::shutdown(sockfd, how) < 0) {
        throw std::system_error{makeError(errno), "shutdown"};
    }
}

/**
 * Receive a message from a socket.
 */
inline ssize_t recv(int sockfd, void* buf, std::size_t len, int flags, std::error_code& ec) noexcept
{
    const auto ret = ::recv(sockfd, buf, len, flags);
    if (ret < 0) {
        ec = makeError(errno);
    }
    return ret;
}

/**
 * Receive a message from a socket.
 */
inline std::size_t recv(int sockfd, void* buf, std::size_t len, int flags)
{
    const auto ret = ::recv(sockfd, buf, len, flags);
    if (ret < 0) {
        throw std::system_error{makeError(errno), "recv"};
    }
    return ret;
}

/**
 * Receive a message from a socket.
 */
inline ssize_t recv(int sockfd, const MutableBuffer& buf, int flags, std::error_code& ec) noexcept
{
    return recv(sockfd, buffer_cast<void*>(buf), buffer_size(buf), flags, ec);
}

/**
 * Receive a message from a socket.
 */
inline std::size_t recv(int sockfd, const MutableBuffer& buf, int flags)
{
    return recv(sockfd, buffer_cast<void*>(buf), buffer_size(buf), flags);
}

/**
 * Receive a message from a socket.
 */
inline ssize_t recvfrom(int sockfd, void* buf, std::size_t len, int flags, sockaddr& addr,
                        socklen_t& addrlen, std::error_code& ec) noexcept
{
    // The addrlen argument is updated to contain the actual size of the source address.
    // The returned address is truncated if the buffer provided is too small.
    const auto ret = ::recvfrom(sockfd, buf, len, flags, &addr, &addrlen);
    if (ret < 0) {
        ec = makeError(errno);
    }
    return ret;
}

/**
 * Receive a message from a socket.
 */
inline std::size_t recvfrom(int sockfd, void* buf, std::size_t len, int flags, sockaddr& addr,
                            socklen_t& addrlen)
{
    // The addrlen argument is updated to contain the actual size of the source address.
    // The returned address is truncated if the buffer provided is too small.
    const auto ret = ::recvfrom(sockfd, buf, len, flags, &addr, &addrlen);
    if (ret < 0) {
        throw std::system_error{makeError(errno), "recvfrom"};
    }
    return ret;
}

/**
 * Receive a message from a socket.
 */
template <typename TransportT>
inline ssize_t recvfrom(int sockfd, void* buf, std::size_t len, int flags,
                        BasicEndpoint<TransportT>& ep, std::error_code& ec) noexcept
{
    socklen_t addrlen = ep.capacity();
    const auto ret = recvfrom(sockfd, buf, len, flags, *ep.data(), addrlen, ec);
    if (!ec) {
        ep.resize(std::min<std::size_t>(addrlen, ep.capacity()));
    }
    return ret;
}

/**
 * Receive a message from a socket.
 */
template <typename TransportT>
inline std::size_t recvfrom(int sockfd, void* buf, std::size_t len, int flags,
                            BasicEndpoint<TransportT>& ep)
{
    socklen_t addrlen = ep.capacity();
    const auto ret = recvfrom(sockfd, buf, len, flags, *ep.data(), addrlen);
    ep.resize(std::min<std::size_t>(addrlen, ep.capacity()));
    return ret;
}

/**
 * Receive a message from a socket.
 */
template <typename TransportT>
inline ssize_t recvfrom(int sockfd, const MutableBuffer& buf, int flags,
                        BasicEndpoint<TransportT>& ep, std::error_code& ec) noexcept
{
    return recvfrom(sockfd, buffer_cast<void*>(buf), buffer_size(buf), flags, ep, ec);
}

/**
 * Receive a message from a socket.
 */
template <typename TransportT>
inline std::size_t recvfrom(int sockfd, const MutableBuffer& buf, int flags,
                            BasicEndpoint<TransportT>& ep)
{
    return recvfrom(sockfd, buffer_cast<void*>(buf), buffer_size(buf), flags, ep);
}

/**
 * Send a message on a socket.
 */
inline ssize_t send(int sockfd, const void* buf, std::size_t len, int flags,
                    std::error_code& ec) noexcept
{
    const auto ret = ::send(sockfd, buf, len, flags);
    if (ret < 0) {
        ec = makeError(errno);
    }
    return ret;
}

/**
 * Send a message on a socket.
 */
inline std::size_t send(int sockfd, const void* buf, std::size_t len, int flags)
{
    const auto ret = ::send(sockfd, buf, len, flags);
    if (ret < 0) {
        throw std::system_error{makeError(errno), "send"};
    }
    return ret;
}

/**
 * Send a message on a socket.
 */
inline ssize_t send(int sockfd, const ConstBuffer& buf, int flags, std::error_code& ec) noexcept
{
    return send(sockfd, buffer_cast<const void*>(buf), buffer_size(buf), flags, ec);
}

/**
 * Send a message on a socket.
 */
inline std::size_t send(int sockfd, const ConstBuffer& buf, int flags)
{
    return send(sockfd, buffer_cast<const void*>(buf), buffer_size(buf), flags);
}

/**
 * Send a message on a socket.
 */
inline ssize_t sendto(int sockfd, const void* buf, std::size_t len, int flags, const sockaddr& addr,
                      socklen_t addrlen, std::error_code& ec) noexcept
{
    const auto ret = ::sendto(sockfd, buf, len, flags, &addr, addrlen);
    if (ret < 0) {
        ec = makeError(errno);
    }
    return ret;
}

/**
 * Send a message on a socket.
 */
inline std::size_t sendto(int sockfd, const void* buf, std::size_t len, int flags,
                          const sockaddr& addr, socklen_t addrlen)
{
    const auto ret = ::sendto(sockfd, buf, len, flags, &addr, addrlen);
    if (ret < 0) {
        throw std::system_error{makeError(errno), "sendto"};
    }
    return ret;
}

/**
 * Send a message on a socket.
 */
template <typename TransportT>
inline ssize_t sendto(int sockfd, const void* buf, std::size_t len, int flags,
                      const BasicEndpoint<TransportT>& ep, std::error_code& ec) noexcept
{
    return sendto(sockfd, buf, len, flags, *ep.data(), ep.size());
}

/**
 * Send a message on a socket.
 */
template <typename TransportT>
inline std::size_t sendto(int sockfd, const void* buf, std::size_t len, int flags,
                          const BasicEndpoint<TransportT>& ep)
{
    return sendto(sockfd, buf, len, flags, *ep.data(), ep.size());
}

/**
 * Send a message on a socket.
 */
template <typename TransportT>
inline ssize_t sendto(int sockfd, const ConstBuffer& buf, int flags,
                      const BasicEndpoint<TransportT>& ep, std::error_code& ec) noexcept
{
    return sendto(sockfd, buffer_cast<const void*>(buf), buffer_size(buf), flags, *ep.data(),
                  ep.size(), ec);
}

/**
 * Send a message on a socket.
 */
template <typename TransportT>
inline std::size_t sendto(int sockfd, const ConstBuffer& buf, int flags,
                          const BasicEndpoint<TransportT>& ep)
{
    return sendto(sockfd, buffer_cast<const void*>(buf), buffer_size(buf), flags, *ep.data(),
                  ep.size());
}

/**
 * Get socket option.
 */
inline void getsockopt(int sockfd, int level, int optname, void* optval, socklen_t& optlen,
                       std::error_code& ec) noexcept
{
    if (::getsockopt(sockfd, level, optname, optval, &optlen) < 0) {
        ec = makeError(errno);
    }
}

/**
 * Get socket option.
 */
inline void getsockopt(int sockfd, int level, int optname, void* optval, socklen_t& optlen)
{
    if (::getsockopt(sockfd, level, optname, optval, &optlen) < 0) {
        throw std::system_error{makeError(errno), "getsockopt"};
    }
}

/**
 * Set socket option.
 */
inline void setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen,
                       std::error_code& ec) noexcept
{
    if (::setsockopt(sockfd, level, optname, optval, optlen) < 0) {
        ec = makeError(errno);
    }
}

/**
 * Set socket option.
 */
inline void setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen)
{
    if (::setsockopt(sockfd, level, optname, optval, optlen) < 0) {
        throw std::system_error{makeError(errno), "setsockopt"};
    }
}

} // namespace sys

inline int getSoError(int sockfd, std::error_code& ec) noexcept
{
    int optval{};
    socklen_t optlen{sizeof(optval)};
    sys::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, optlen);
    return optval;
}

inline int getSoError(int sockfd)
{
    int optval{};
    socklen_t optlen{sizeof(optval)};
    sys::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, optlen);
    return optval;
}

inline void setSoReuseAddr(int sockfd, bool enabled, std::error_code& ec) noexcept
{
    int optval{enabled ? 1 : 0};
    sys::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval), ec);
}

inline void setSoReuseAddr(int sockfd, bool enabled)
{
    int optval{enabled ? 1 : 0};
    sys::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

inline void setSoRcvBuf(int sockfd, int size, std::error_code& ec) noexcept
{
    sys::setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size), ec);
}

inline void setSoRcvBuf(int sockfd, int size)
{
    sys::setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
}

inline void setSoSndBuf(int sockfd, int size, std::error_code& ec) noexcept
{
    sys::setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size), ec);
}

inline void setSoSndBuf(int sockfd, int size)
{
    sys::setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
}

inline void setTcpNoDelay(int sockfd, bool enabled, std::error_code& ec) noexcept
{
    int optval{enabled ? 1 : 0};
    sys::setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval), ec);
}

inline void setTcpNoDelay(int sockfd, bool enabled)
{
    int optval{enabled ? 1 : 0};
    sys::setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

struct Socket {
    Socket(File&& sock, int family)
      : sock_{std::move(sock)}
      , family_{family}
    {
    }
    Socket() noexcept = default;

    auto operator*() const noexcept { return *sock_; }
    auto get() const noexcept { return sock_.get(); }
    explicit operator bool() const noexcept { return static_cast<bool>(sock_); }
    int family() const noexcept { return family_; }

    // Logically const.
    int getSoError() const { return swirly::getSoError(*sock_); }

    void close() { sock_.reset(); }

    void setNonBlock(std::error_code& ec) noexcept { swirly::setNonBlock(*sock_, ec); }
    void setNonBlock() { swirly::setNonBlock(*sock_); }

    void setSoRcvBuf(int size, std::error_code& ec) noexcept
    {
        swirly::setSoRcvBuf(*sock_, size, ec);
    }
    void setSoRcvBuf(int size) { swirly::setSoRcvBuf(*sock_, size); }

    void setSoReuseAddr(bool enabled, std::error_code& ec) noexcept
    {
        swirly::setSoReuseAddr(*sock_, enabled, ec);
    }
    void setSoReuseAddr(bool enabled) { swirly::setSoReuseAddr(*sock_, enabled); }

    void setSoSndBuf(int size, std::error_code& ec) noexcept
    {
        swirly::setSoSndBuf(*sock_, size, ec);
    }
    void setSoSndBuf(int size) { swirly::setSoSndBuf(*sock_, size); }

    void setTcpNoDelay(bool enabled, std::error_code& ec) noexcept
    {
        swirly::setTcpNoDelay(*sock_, enabled, ec);
    }
    void setTcpNoDelay(bool enabled) { swirly::setTcpNoDelay(*sock_, enabled); }

  protected:
    File sock_;
    int family_{};
};

} // namespace swirly

#endif // SWIRLY_SYS_SOCKET_HPP
