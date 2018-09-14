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
#include <swirly/http/App.hpp>
#include <swirly/http/Serv.hpp>

#include <swirly/app/Thread.hpp>

#include <swirly/sys/EpollReactor.hpp>
#include <swirly/sys/Signal.hpp>

using namespace std;
using namespace swirly;

namespace {

void on_foo(const HttpRequest& req, HttpStream& os)
{
    os << "Hello, Foo!";
}

void on_bar(const HttpRequest& req, HttpStream& os)
{
    os << "Hello, Bar!";
}

class TestApp : public HttpApp {
  public:
    using Slot = BasicSlot<const HttpRequest&, HttpStream&>;
    using SlotMap = std::unordered_map<std::string, Slot>;

    void bind(const std::string& path, Slot slot) { slot_map_[path] = slot; }

  protected:
    void do_on_connect(Time now, HttpConn& conn) noexcept override
    {
        SWIRLY_INFO << "session connected: " << conn.endpoint();
    }
    void do_on_disconnect(Time now, HttpConn& conn) noexcept override
    {
        SWIRLY_INFO << "session disconnected: " << conn.endpoint();
    }
    void do_on_error(Time now, HttpConn& conn, const std::exception& e) noexcept override
    {
        SWIRLY_ERROR << "session error: " << conn.endpoint() << ": " << e.what();
    }
    void do_on_message(Time now, HttpConn& conn, const HttpRequest& req, HttpStream& os) override
    {
        const auto it = slot_map_.find(string{req.path()});
        if (it != slot_map_.end()) {
            os.reset(HttpStatus::Ok, TextPlain);
            it->second(req, os);
        } else {
            os.reset(HttpStatus::NotFound, TextPlain);
            os << "Error 404 - Page not found";
        }
        os.commit();
    }
    void do_on_timeout(Time now, HttpConn& conn) noexcept override
    {
        SWIRLY_WARNING << "session timeout: " << conn.endpoint();
    }

  private:
    SlotMap slot_map_;
};

} // namespace

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        const auto start_time = UnixClock::now();

        EpollReactor reactor{1024};
        TestApp app;
        app.bind("/foo", bind<on_foo>());
        app.bind("/bar", bind<on_bar>());

        const TcpEndpoint ep{Tcp::v4(), 8888};
        HttpServ http_serv{start_time, reactor, ep, app};

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
