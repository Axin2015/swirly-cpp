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
#ifndef SWIRLY_SYS_TCPSOCKET_HPP
#define SWIRLY_SYS_TCPSOCKET_HPP

#include <swirly/sys/IoSocket.hpp>

namespace swirly {
inline namespace sys {

/**
 * Passive TCP Server Socket. All state is in base class, so object can be sliced.
 */
struct TcpServSocket : Socket {
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    using Socket::Socket;

    TcpServSocket(Transport trans, std::error_code& ec) noexcept
    : Socket{os::socket(trans, ec), trans.family()}
    {
    }
    explicit TcpServSocket(Transport trans)
    : Socket{os::socket(trans), trans.family()}
    {
    }
    TcpServSocket() noexcept = default;

    // Logically const.
    void get_sock_name(Endpoint& ep, std::error_code& ec) noexcept
    {
        os::getsockname(*sock_, ep, ec);
    }
    void get_sock_name(Endpoint& ep) { os::getsockname(*sock_, ep); }
    void bind(const Endpoint& ep, std::error_code& ec) noexcept { os::bind(*sock_, ep, ec); }
    void bind(const Endpoint& ep) { os::bind(*sock_, ep); }

    void listen(int backlog, std::error_code& ec) noexcept { os::listen(*sock_, backlog, ec); }
    void listen(int backlog) { os::listen(*sock_, backlog); }

    IoSocket accept(Endpoint& ep, std::error_code& ec) noexcept
    {
        return IoSocket{os::accept(*sock_, ep, ec), family_};
    }
    IoSocket accept(Endpoint& ep) { return IoSocket{os::accept(*sock_, ep), family_}; }
};

/**
 * Active TCP Client Socket. All state is in base class, so object can be sliced.
 */
struct TcpClntSocket : IoSocket {
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    using IoSocket::IoSocket;

    TcpClntSocket(Transport trans, std::error_code& ec) noexcept
    : IoSocket{os::socket(trans, ec), trans.family()}
    {
    }
    explicit TcpClntSocket(Transport trans)
    : IoSocket{os::socket(trans), trans.family()}
    {
    }
    TcpClntSocket() noexcept = default;

    // Logically const.
    void get_sock_name(Endpoint& ep, std::error_code& ec) noexcept
    {
        os::getsockname(*sock_, ep, ec);
    }
    void get_sock_name(Endpoint& ep) { os::getsockname(*sock_, ep); }
    void connect(const Endpoint& ep, std::error_code& ec) noexcept
    {
        return os::connect(*sock_, ep, ec);
    }
    void connect(const Endpoint& ep) { return os::connect(*sock_, ep); }
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_TCPSOCKET_HPP
