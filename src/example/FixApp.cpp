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
#include <swirly/fix/App.hpp>
#include <swirly/fix/Ctx.hpp>
#include <swirly/fix/Lexer.hpp>

#include <swirly/app/Thread.hpp>

#include <swirly/sys/EpollReactor.hpp>
#include <swirly/sys/Signal.hpp>

#include <swirly/util/Log.hpp>

#include <iostream>

using namespace std;
using namespace swirly;

namespace {

class TestApp : public FixApp {
  public:
  protected:
    void do_on_connect(Time now, FixConn& conn) override
    {
        SWIRLY_INFO << "session connected: " << conn.endpoint();
    }
    void do_on_logon(Time now, FixConn& conn, const FixSessId& sess_id) override
    {
        SWIRLY_INFO << "session logged-on: " << conn.endpoint();
    }
    void do_on_logout(Time now, FixConn& conn, const FixSessId& sess_id) noexcept override
    {
        SWIRLY_INFO << "session logged-out: " << conn.endpoint();
    }
    void do_on_message(Time now, FixConn& conn, string_view msg, size_t body_off, Version ver,
                       const FixHeader& hdr) override
    {
        FixLexer lex{msg};
        while (!lex.empty()) {
            const auto [t, v] = lex.next();
            cout << t << '=' << v << '^';
        }
        cout << endl;
    }
    void do_on_disconnect(Time now, const FixConn& conn) noexcept override
    {
        SWIRLY_INFO << "session disconnected: " << conn.endpoint();
    }
    void do_on_error(Time now, const FixConn& conn, const std::exception& e) noexcept override
    {
        SWIRLY_ERROR << "session error: " << conn.endpoint() << ": " << e.what();
    }
    void do_on_timeout(Time now, const FixConn& conn) noexcept override
    {
        SWIRLY_WARNING << "session timeout: " << conn.endpoint();
    }
};

constexpr char ConfigData[] = R"(
version=4.3
heart_bt_int=2

[session]
type=initiator
endpoint=0.0.0.0:5002
sender_comp_id=CLIENT1
target_comp_id=SERVER1

[session]
type=initiator
endpoint=0.0.0.0:5002
sender_comp_id=CLIENT2
target_comp_id=SERVER1

[session]
type=acceptor
endpoint=127.0.0.1:5002
sender_comp_id=SERVER1
target_comp_id=CLIENT1

[session]
type=acceptor
endpoint=127.0.0.1:5002
sender_comp_id=SERVER1
target_comp_id=CLIENT2
)";

} // namespace

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        EpollReactor reactor{1024};
        istringstream is{ConfigData};
        TestApp app;

        const auto start_time = UnixClock::now();
        FixCtx fix_ctx{start_time, reactor, is, app};

        // Start service/worker threads.
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
