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
#ifndef SWIRLY_SYS_TCPACCEPTOR_HPP
#define SWIRLY_SYS_TCPACCEPTOR_HPP

#include <swirly/sys/Event.hpp>
#include <swirly/sys/Reactor.hpp>
#include <swirly/sys/TcpSocket.hpp>

namespace swirly {
inline namespace sys {

template <typename DerivedT>
class TcpAcceptor {
  public:
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    TcpAcceptor(Reactor& r, const Endpoint& ep)
    : serv_{ep.protocol()}
    {
        serv_.set_so_reuse_addr(true);
        serv_.bind(ep);
        serv_.listen(SOMAXCONN);
        sub_ = r.subscribe(*serv_, EventIn, bind<&TcpAcceptor::on_io_event>(this));
    }

    // Copy.
    TcpAcceptor(const TcpAcceptor&) = delete;
    TcpAcceptor& operator=(const TcpAcceptor&) = delete;

    // Move.
    TcpAcceptor(TcpAcceptor&&) = delete;
    TcpAcceptor& operator=(TcpAcceptor&&) = delete;

  protected:
    ~TcpAcceptor() = default;

  private:
    void on_io_event(Time now, int fd, unsigned events)
    {
        Endpoint ep;
        IoSocket sock{os::accept(fd, ep), serv_.family()};
        static_cast<DerivedT*>(this)->do_accept(now, std::move(sock), ep);
    }

    TcpServSocket serv_;
    Reactor::Handle sub_;
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_TCPACCEPTOR_HPP
