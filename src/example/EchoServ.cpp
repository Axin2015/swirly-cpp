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
#include <swirly/app/Thread.hpp>

#include <swirly/sys/EpollReactor.hpp>
#include <swirly/sys/Signal.hpp>
#include <swirly/sys/TcpAcceptor.hpp>

#include <swirly/util/Log.hpp>
#include <swirly/util/Tokeniser.hpp>

#include <boost/intrusive/list.hpp>

using namespace std;
using namespace swirly;

namespace {

constexpr auto IdleTimeout = 5s;

class EchoConn {

    // Automatically unlink when object is destroyed.
    using AutoUnlinkOption = boost::intrusive::link_mode<boost::intrusive::auto_unlink>;

  public:
    EchoConn(Time now, Reactor& r, IoSocket&& sock, const TcpEndpoint& ep)
    : reactor_(r)
    , sock_{move(sock)}
    , ep_{ep}
    {
        sub_ = r.subscribe(sock_.get(), EventIn, bind<&EchoConn::on_input>(this));
        tmr_ = r.timer(now + IdleTimeout, Priority::Low, bind<&EchoConn::on_timer>(this));
    }
    boost::intrusive::list_member_hook<AutoUnlinkOption> list_hook;

  private:
    void dispose(Time now) noexcept
    {
        SWIRLY_INFO << "connection closed";
        delete this;
    }
    void on_input(Time now, int fd, unsigned events)
    {
        try {
            if (events & (EventIn | EventHup)) {

                const auto size = os::read(fd, buf_.prepare(2048));
                if (size > 0) {
                    // Commit actual bytes read.
                    buf_.commit(size);

                    // Parse each buffered line.
                    auto fn = [fd](Time now, std::string_view line) {
                        // Echo bytes back to client.
                        std::string buf{line};
                        buf += '\n';
                        if (os::write(fd, {buf.data(), buf.size()}) < buf.size()) {
                            throw runtime_error{"partial write"};
                        }
                    };
                    buf_.consume(parseLine(now, buf_.str(), fn));

                    // Reset timer.
                    tmr_.cancel();
                    tmr_ = reactor_.timer(now + IdleTimeout, Priority::Low,
                                          bind<&EchoConn::on_timer>(this));
                } else {
                    dispose(now);
                }
            }
        } catch (const std::exception& e) {
            SWIRLY_ERROR << "exception: " << e.what();
            dispose(now);
        }
    }
    void on_timer(Time now, Timer& tmr)
    {
        SWIRLY_INFO << "timeout";
        dispose(now);
    }
    Reactor& reactor_;
    IoSocket sock_;
    const TcpEndpoint ep_;
    Reactor::Handle sub_;
    Buffer buf_;
    Timer tmr_;
};

class EchoServ : public TcpAcceptor<EchoServ> {

    friend TcpAcceptor<EchoServ>;
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using MemberHookOption = boost::intrusive::member_hook<EchoConn, decltype(EchoConn::list_hook),
                                                           &EchoConn::list_hook>;
    using ConnList = boost::intrusive::list<EchoConn, ConstantTimeSizeOption, MemberHookOption>;

  public:
    EchoServ(Time now, Reactor& r, const Endpoint& ep)
    : TcpAcceptor{r, ep}
    , reactor_(r)
    {
    }
    ~EchoServ()
    {
        conn_list_.clear_and_dispose([](auto* conn) { delete conn; });
    }

  private:
    void do_accept(Time now, IoSocket&& sock, const Endpoint& ep)
    {
        SWIRLY_INFO << "connection opened: " << ep;
        sock.set_non_block();
        sock.set_tcp_no_delay(true);
        // High performance TCP servers could use a custom allocator.
        auto* const conn = new EchoConn{now, reactor_, move(sock), ep};
        conn_list_.push_back(*conn);
    }
    Reactor& reactor_;
    // List of active connections.
    ConnList conn_list_;
};
} // namespace

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        const auto start_time = UnixClock::now();

        EpollReactor reactor{1024};
        const TcpEndpoint ep{Tcp::v4(), 7777};
        EchoServ echo_serv{start_time, reactor, ep};

        // Start service threads.
        ReactorThread reactor_thread{reactor, ThreadConfig{"reactor"s}};

        // Wait for termination.
        SigWait sig_wait;
        while (const auto sig = sig_wait()) {
            switch (sig) {
            case SIGHUP:
                SWIRLY_INFO << "received SIGHUP";
                continue;
            case SIGINT:
                SWIRLY_INFO << "received SIGINT";
                break;
            case SIGTERM:
                SWIRLY_INFO << "received SIGTERM";
                break;
            default:
                SWIRLY_INFO << "received signal: " << sig;
                continue;
            }
            break;
        }
        ret = 0;

    } catch (const std::exception& e) {
        SWIRLY_ERROR << "exception: " << e.what();
    }
    return ret;
}
