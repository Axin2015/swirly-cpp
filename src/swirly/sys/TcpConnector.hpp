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
#ifndef SWIRLY_SYS_TCPCONNECTOR_HPP
#define SWIRLY_SYS_TCPCONNECTOR_HPP

#include <swirly/sys/Event.hpp>
#include <swirly/sys/Reactor.hpp>
#include <swirly/sys/TcpSocket.hpp>

namespace swirly {
inline namespace sys {

template <typename DerivedT>
class TcpConnector {
  public:
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    TcpConnector() = default;

    // Copy.
    TcpConnector(const TcpConnector&) = delete;
    TcpConnector& operator=(const TcpConnector&) = delete;

    // Move.
    TcpConnector(TcpConnector&&) = delete;
    TcpConnector& operator=(TcpConnector&&) = delete;

    /*
     * Returns true if connection was established synchronously or false if connection is pending
     * asynchronous completion.
     */
    bool connect(Reactor& r, const Endpoint& ep, Time now) noexcept
    {
        TcpClntSocket sock{ep.protocol()};
        sock.set_non_block();
        sock.set_tcp_no_delay(true);

        std::error_code ec;
        sock.connect(ep, ec);
        if (ec) {
            if (ec.value() != EINPROGRESS) {
                throw std::system_error{ec, "connect"};
            }
            sub_ = r.subscribe(*sock, EventOut, bind<&TcpConnector::on_io_event>(this));
            ep_ = ep;
            sock_ = std::move(sock);
            return false;
        }
        static_cast<DerivedT*>(this)->do_connect(std::move(sock), ep, now);
        return true;
    }

  protected:
    ~TcpConnector() = default;

  private:
    void on_io_event(int fd, unsigned events, Time now)
    {
        assert(events == EventOut);
        IoSocket sock{std::move(sock_)};
        sub_.reset();
        try {
            const auto ec = sock.get_so_error();
            if (ec) {
                throw std::system_error{ec, "connect"};
            }
            static_cast<DerivedT*>(this)->do_connect(std::move(sock), ep_, now);
        } catch (const std::exception& e) {
            static_cast<DerivedT*>(this)->do_connect_error(e, now);
        }
    }

    Endpoint ep_;
    TcpClntSocket sock_;
    Reactor::Handle sub_;
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_TCPCONNECTOR_HPP
