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
        sock_.setNonBlock();
        sock_.setTcpNoDelay(true);
        sub_ = r.subscribe(sock_.get(), EventIn, bind<&EchoSess::onInput>(this));
        tmr_ = r.timer(now + IdleTimeout, Priority::Low, bind<&EchoSess::onTimer>(this));
    }
    void close() noexcept
    {
        SWIRLY_INFO << "session closed"sv;
        tmr_.cancel();
        sub_.reset();
        delete this;
    }
    void onInput(int fd, unsigned events, Time now)
    {
        try {
            if (events & EventIn) {
                char buf[2048];
                const auto size = os::read(fd, buf, sizeof(buf));
                if (size > 0) {
                    assert(size > 0);
                    // Echo bytes back to client.
                    if (os::write(fd, buf, size) < size) {
                        throw runtime_error{"Partial write"};
                    }
                    tmr_.cancel();
                    tmr_ = reactor_.timer(now + IdleTimeout, Priority::Low,
                                          bind<&EchoSess::onTimer>(this));
                } else {
                    close();
                }
            }
        } catch (const std::exception& e) {
            SWIRLY_ERROR << "exception: "sv << e.what();
            close();
        }
    }
    void onTimer(Timer& tmr, Time now)
    {
        SWIRLY_INFO << "timeout"sv;
        close();
    }
    boost::intrusive::list_member_hook<AutoUnlinkOption> listHook;

  private:
    Reactor& reactor_;
    IoSocket sock_;
    TcpEndpoint ep_;
    Reactor::Handle sub_;
    Timer tmr_;
};

class EchoServ : public TcpAcceptor<EchoServ> {

    friend TcpAcceptor<EchoServ>;
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using MemberHookOption = boost::intrusive::member_hook<EchoSess, decltype(EchoSess::listHook),
                                                           &EchoSess::listHook>;
    using SessList = boost::intrusive::list<EchoSess, ConstantTimeSizeOption, MemberHookOption>;

  public:
    EchoServ(Reactor& r, const Endpoint& ep)
    : TcpAcceptor{r, ep}
    , reactor_(r)
    {
    }
    ~EchoServ()
    {
        sessList_.clear_and_dispose([](auto* sess) { delete sess; });
    }

  private:
    void doAccept(IoSocket&& sock, const Endpoint& ep, Time now)
    {
        SWIRLY_INFO << "session opened: "sv << ep;
        // High performance TCP servers could use a custom allocator.
        auto* const sess = new EchoSess{reactor_, move(sock), ep, now};
        sessList_.push_back(*sess);
    }
    Reactor& reactor_;
    // List of active sessions.
    SessList sessList_;
};
} // namespace

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        EpollReactor reactor{1024};
        const TcpEndpoint ep{Tcp::v4(), 7777};
        EchoServ echoServ{reactor, ep};

        // Start service threads.
        ReactorThread reactorThread{reactor, ThreadConfig{"reactor"s}};

        // Wait for termination.
        SigWait sigWait;
        while (const auto sig = sigWait()) {
            switch (sig) {
            case SIGHUP:
                SWIRLY_INFO << "received SIGHUP"sv;
                continue;
            case SIGINT:
                SWIRLY_INFO << "received SIGINT"sv;
                break;
            case SIGTERM:
                SWIRLY_INFO << "received SIGTERM"sv;
                break;
            default:
                SWIRLY_INFO << "received signal: "sv << sig;
                continue;
            }
            break;
        }
        ret = 0;

    } catch (const std::exception& e) {
        SWIRLY_ERROR << "exception: "sv << e.what();
    }
    return ret;
}
