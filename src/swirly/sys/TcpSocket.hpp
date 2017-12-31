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

/**
 * Passive TCP Server Socket. All state is in base class, so object can be sliced.
 */
struct TcpSocketServ : Socket {
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    using Socket::Socket;

    TcpSocketServ(Transport trans, std::error_code& ec) noexcept
      : Socket{sys::socket(trans, ec), trans.family()}
    {
    }
    explicit TcpSocketServ(Transport trans)
      : Socket{sys::socket(trans), trans.family()}
    {
    }

    TcpSocketServ() noexcept = default;

    void bind(const Endpoint& ep, std::error_code& ec) noexcept { sys::bind(*sock_, ep, ec); }
    void bind(const Endpoint& ep) { sys::bind(*sock_, ep); }

    void listen(int backlog, std::error_code& ec) noexcept { sys::listen(*sock_, backlog, ec); }
    void listen(int backlog) { sys::listen(*sock_, backlog); }

    IoSocket accept(Endpoint& ep, std::error_code& ec) noexcept
    {
        return IoSocket{sys::accept(*sock_, ep, ec), family_};
    }
    IoSocket accept(Endpoint& ep) { return IoSocket{sys::accept(*sock_, ep), family_}; }
};

/**
 * Active TCP Client Socket. All state is in base class, so object can be sliced.
 */
struct TcpSocketClnt : IoSocket {
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    using IoSocket::IoSocket;

    TcpSocketClnt(Transport trans, std::error_code& ec) noexcept
      : IoSocket{sys::socket(trans, ec), trans.family()}
    {
    }
    explicit TcpSocketClnt(Transport trans)
      : IoSocket{sys::socket(trans), trans.family()}
    {
    }

    TcpSocketClnt() noexcept = default;

    void connect(const Endpoint& ep, std::error_code& ec) noexcept
    {
        return sys::connect(*sock_, ep, ec);
    }
    void connect(const Endpoint& ep) { return sys::connect(*sock_, ep); }
};

} // namespace swirly

#endif // SWIRLY_SYS_TCPSOCKET_HPP
