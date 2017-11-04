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
#ifndef SWIRLY_SYS_IOSOCKET_HPP
#define SWIRLY_SYS_IOSOCKET_HPP

#include <swirly/sys/Socket.hpp>

namespace swirly {

/**
 * Socket with IO operations. I.e. not a passive listener. All state is in base class, so object can
 * be sliced.
 */
struct IoSocket : Socket {
    using Socket::Socket;

    IoSocket() = default;

    void shutdown(int how) { return sys::shutdown(*sock_, how); }

    ssize_t recv(void* buf, std::size_t len, int flags, std::error_code& ec) noexcept
    {
        return sys::recv(*sock_, buf, len, flags, ec);
    }
    std::size_t recv(void* buf, std::size_t len, int flags)
    {
        return sys::recv(*sock_, buf, len, flags);
    }

    ssize_t recv(const MutableBuffer& buf, int flags, std::error_code& ec) noexcept
    {
        return sys::recv(*sock_, buf, flags, ec);
    }
    std::size_t recv(const MutableBuffer& buf, int flags) { return sys::recv(*sock_, buf, flags); }

    ssize_t send(const void* buf, std::size_t len, int flags, std::error_code& ec) noexcept
    {
        return sys::send(*sock_, buf, len, flags, ec);
    }
    std::size_t send(const void* buf, std::size_t len, int flags)
    {
        return sys::send(*sock_, buf, len, flags);
    }

    ssize_t send(const ConstBuffer& buf, int flags, std::error_code& ec) noexcept
    {
        return sys::send(*sock_, buf, flags, ec);
    }
    std::size_t send(const ConstBuffer& buf, int flags) { return sys::send(*sock_, buf, flags); }
};

template <typename TransportT>
inline std::pair<IoSocket, IoSocket> socketpair(TransportT trans)
{
    auto socks = sys::socketpair(trans);
    return {IoSocket{std::move(socks.first), trans.family()},
            IoSocket{std::move(socks.second), trans.family()}};
}

} // namespace swirly

#endif // SWIRLY_SYS_IOSOCKET_HPP
