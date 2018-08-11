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

#include <boost/intrusive/list.hpp>

using namespace std;
using namespace swirly;

namespace {

constexpr auto IdleTimeout = 5s;

class EchoSess {

    // Automatically unlink when object is destroyed.
    using AutoUnlinkOption = boost::intrusive::link_mode<boost::intrusive::auto_unlink>;

  public:
    EchoSess(Reactor& r, IoSocket&& sock, const TcpEndpoint& ep, Time now)
    : reactor_(r)
    , sock_{move(sock)}
    , ep_{ep}
    {
        sub_ = r.subscribe(sock_.get(), EventIn, bind<&EchoSess::on_input>(this));
        tmr_ = r.timer(now + IdleTimeout, Priority::Low, bind<&EchoSess::on_timer>(this));
    }
    boost::intrusive::list_member_hook<AutoUnlinkOption> list_hook;

  private:
    void dispose() noexcept
    {
        SWIRLY_INFO << "session closed";
        delete this;
    }
    void on_input(int fd, unsigned events, Time now)
    {
        try {
            if (events & EventIn) {
                char buf[2048];
                const auto size = os::read(fd, buf, sizeof(buf));
                if (size > 0) {
                    assert(size > 0);
                    // Echo bytes back to client.
                    if (os::write(fd, buf, size) < size) {
                        throw runtime_error{"partial write"};
                    }
                    tmr_.cancel();
                    tmr_ = reactor_.timer(now + IdleTimeout, Priority::Low,
                                          bind<&EchoSess::on_timer>(this));
                } else {
                    dispose();
                }
            }
        } catch (const std::exception& e) {
            SWIRLY_ERROR << "exception: " << e.what();
            dispose();
        }
    }
    void on_timer(Timer& tmr, Time now)
    {
        SWIRLY_INFO << "timeout";
        dispose();
    }
    Reactor& reactor_;
    IoSocket sock_;
    const TcpEndpoint ep_;
    Reactor::Handle sub_;
    Timer tmr_;
};

class EchoServ : public TcpAcceptor<EchoServ> {

    friend TcpAcceptor<EchoServ>;
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using MemberHookOption = boost::intrusive::member_hook<EchoSess, decltype(EchoSess::list_hook),
                                                           &EchoSess::list_hook>;
    using SessList = boost::intrusive::list<EchoSess, ConstantTimeSizeOption, MemberHookOption>;

  public:
    EchoServ(Reactor& r, const Endpoint& ep, Time now)
    : TcpAcceptor{r, ep}
    , reactor_(r)
    {
    }
    ~EchoServ()
    {
        sess_list_.clear_and_dispose([](auto* sess) { delete sess; });
    }

  private:
    void do_accept(IoSocket&& sock, const Endpoint& ep, Time now)
    {
        SWIRLY_INFO << "session opened: " << ep;
        sock.set_non_block();
        sock.set_tcp_no_delay(true);
        // High performance TCP servers could use a custom allocator.
        auto* const sess = new EchoSess{reactor_, move(sock), ep, now};
        sess_list_.push_back(*sess);
    }
    Reactor& reactor_;
    // List of active sessions.
    SessList sess_list_;
};
} // namespace

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        const auto start_time = UnixClock::now();

        EpollReactor reactor{1024};
        const TcpEndpoint ep{Tcp::v4(), 7777};
        EchoServ echo_serv{reactor, ep, start_time};

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
