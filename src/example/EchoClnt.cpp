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
#include <swirly/sys/TcpConnector.hpp>

#include <swirly/util/Log.hpp>
#include <swirly/util/Tokeniser.hpp>

#include <boost/intrusive/list.hpp>

using namespace std;
using namespace swirly;

namespace {

constexpr auto PingInterval = 100ms;

class EchoConn {

    // Automatically unlink when object is destroyed.
    using AutoUnlinkOption = boost::intrusive::link_mode<boost::intrusive::auto_unlink>;

  public:
    EchoConn(Time now, Reactor& r, IoSocket&& sock, const TcpEndpoint& ep)
    : sock_{move(sock)}
    , ep_{ep}
    {
        sub_ = r.subscribe(sock_.get(), EventIn, bind<&EchoConn::on_input>(this));
        tmr_ = r.timer(now, PingInterval, Priority::Low, bind<&EchoConn::on_timer>(this));
    }
    void dispose(Time now) noexcept
    {
        SWIRLY_INFO << "connection closed";
        delete this;
    }
    boost::intrusive::list_member_hook<AutoUnlinkOption> list_hook;

  private:
    ~EchoConn() = default;
    void on_input(Time now, int fd, unsigned events)
    {
        try {
            if (events & (EventIn | EventHup)) {
                const auto size = os::read(fd, buf_.prepare(2048));
                if (size > 0) {
                    // Commit actual bytes read.
                    buf_.commit(size);

                    // Parse each buffered line.
                    auto fn = [this](Time now, std::string_view line) {
                        ++count_;
                        // Echo bytes back to client.
                        SWIRLY_INFO << "received: " << line;
                    };
                    buf_.consume(parse_line(now, buf_.str(), fn));
                    if (count_ == 5) {
                        dispose(now);
                    }
                } else {
                    dispose(now);
                }
            }
        } catch (const std::exception& e) {
            SWIRLY_ERROR << "failed to read data: " << e.what();
            dispose(now);
        }
    }
    void on_timer(Time now, Timer& tmr)
    {
        try {
            if (sock_.send("ping\n", 5, 0) < 5) {
                throw runtime_error{"partial write"};
            }
        } catch (const std::exception& e) {
            SWIRLY_ERROR << "failed to write data: " << e.what();
            dispose(now);
        }
    }
    IoSocket sock_;
    const TcpEndpoint ep_;
    Reactor::Handle sub_;
    Buffer buf_;
    Timer tmr_;
    int count_{0};
};

class EchoClnt : public TcpConnector<EchoClnt> {

    friend TcpConnector<EchoClnt>;
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using MemberHookOption = boost::intrusive::member_hook<EchoConn, decltype(EchoConn::list_hook),
                                                           &EchoConn::list_hook>;
    using ConnList = boost::intrusive::list<EchoConn, ConstantTimeSizeOption, MemberHookOption>;

  public:
    EchoClnt(Time now, Reactor& r, const Endpoint& ep)
    : reactor_(r)
    , ep_(ep)
    {
        // Immediate and then at 2s intervals.
        tmr_ = reactor_.timer(now, 2s, Priority::Low, bind<&EchoClnt::on_timer>(this));
    }
    ~EchoClnt()
    {
        const auto now = UnixClock::now();
        conn_list_.clear_and_dispose([now](auto* conn) { conn->dispose(now); });
    }

  private:
    void do_connect(Time now, IoSocket&& sock, const Endpoint& ep)
    {
        SWIRLY_INFO << "connection opened: " << ep;
        inprogress_ = false;

        // High performance TCP servers could use a custom allocator.
        auto* const conn = new EchoConn{now, reactor_, move(sock), ep};
        conn_list_.push_back(*conn);
    }
    void do_connect_error(Time now, const std::exception& e)
    {
        SWIRLY_ERROR << "failed to connect: " << e.what();
        inprogress_ = false;
    }
    void on_timer(Time now, Timer& tmr)
    {
        if (conn_list_.empty() && !inprogress_) {
            SWIRLY_INFO << "reconnecting";
            if (!connect(now, reactor_, ep_)) {
                inprogress_ = true;
            }
        }
    }
    Reactor& reactor_;
    const Endpoint ep_;
    Timer tmr_;
    bool inprogress_{false};
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
        EchoClnt echo_clnt{start_time, reactor, parse_endpoint<Tcp>("127.0.0.1:7777")};

        // Start service threads.
        ReactorThread reactor_thread{reactor, ThreadConfig{"reactor"s}};

        // Wait for termination.
        SigWait sig_wait;
        for (;;) {
            switch (const auto sig = sig_wait()) {
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
