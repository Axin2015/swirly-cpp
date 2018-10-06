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
#include <swirly/fix/Random.hpp>

#include <swirly/app/Thread.hpp>

#include <swirly/sys/EpollReactor.hpp>
#include <swirly/sys/Signal.hpp>

#include <swirly/util/Log.hpp>

#include <iostream>

using namespace std;
using namespace swirly;

namespace {

class FixHandler;
using FixHandlerPtr = unique_ptr<FixHandler>;
using FixHandlerMap = unordered_map<FixSessId, FixHandlerPtr>;

class FixHandler : public FixHandlerBase {
  public:
    FixHandler() noexcept = default;
    ~FixHandler() override = default;

    // Copy.
    FixHandler(const FixHandler&) noexcept = default;
    FixHandler& operator=(const FixHandler&) noexcept = default;

    // Move.
    FixHandler(FixHandler&&) noexcept = default;
    FixHandler& operator=(FixHandler&&) noexcept = default;

    void prepare(CyclTime now, const FixSessId& sess_id, const FixHandlerMap& handler_map)
    {
        do_prepare(now, sess_id, handler_map);
    }
    void send(CyclTime now, string_view msg_type, string_view msg) { do_send(now, msg_type, msg); }

  protected:
    virtual void do_prepare(CyclTime now, const FixSessId& sess_id,
                            const FixHandlerMap& handler_map)
        = 0;
    virtual void do_send(CyclTime now, string_view msg_type, string_view msg) = 0;
};

class PingHandler : public FixHandler {
  public:
    PingHandler(CyclTime now, Reactor& r, const FixSessId& sess_id, const Config& config)
    : reactor_(r)
    {
    }
    ~PingHandler() override = default;

    // Copy.
    PingHandler(const PingHandler&) = delete;
    PingHandler& operator=(const PingHandler&) = delete;

    // Move.
    PingHandler(PingHandler&&) = delete;
    PingHandler& operator=(PingHandler&&) = delete;

  protected:
    void do_on_logon(CyclTime now, FixConn& conn, const FixSessId& sess_id) override
    {
        SWIRLY_INFO << sess_id << " <Ping> on_logon";
        conn_ = &conn;
        md_tmr_ = reactor_.timer(now.mono_time() + 250ms, 250ms, Priority::Low,
                                 bind<&PingHandler::on_market_data>(this));
        stat_tmr_ = reactor_.timer(now.mono_time() + 1s, 1s, Priority::Low,
                                   bind<&PingHandler::on_status>(this));
    }
    void do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                      Disconnect disconnect) noexcept override
    {
        if (disconnect == Disconnect::No) {
            SWIRLY_INFO << sess_id << " <Ping> on_logout";
        } else {
            SWIRLY_WARNING << sess_id << " <Ping> on_logout";
        }
        count_ = 0;
        stat_tmr_.cancel();
        md_tmr_.cancel();
        conn_ = nullptr;
        conn.logon(now, sess_id);
    }
    void do_on_message(CyclTime now, FixConn& conn, string_view msg, size_t body_off, Version ver,
                       const FixHeader& hdr) override
    {
        SWIRLY_INFO << conn.sess_id() << " <Ping> on_message: " << hdr.msg_type.value;
    }
    void do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept override
    {
        SWIRLY_ERROR << conn.sess_id() << " <Ping> on_error: " << e.what();
    }
    void do_on_timeout(CyclTime now, const FixConn& conn) noexcept override
    {
        SWIRLY_WARNING << conn.sess_id() << " <Ping> on_timeout";
    }
    void do_prepare(CyclTime now, const FixSessId& sess_id,
                    const FixHandlerMap& handler_map) override
    {
        SWIRLY_INFO << sess_id << " <Ping> prepare";
    }
    void do_send(CyclTime now, string_view msg_type, string_view msg) override {}

  private:
    void on_market_data(CyclTime now, Timer& tmr)
    {
        assert(conn_);
        SWIRLY_INFO << conn_->sess_id() << " <Ping> on_market_data";

        auto fn = [this](CyclTime now, ostream& os) {
            const auto [bid, offer] = bbo_(12345);
            // clang-format off
            os << NoMdEntries{4}
               << MdEntryType{'0'}
               << MdEntryPx{bid - 1}
               << MdEntrySize{2000}
               << MdEntryType{'0'}
               << MdEntryPx{bid}
               << MdEntrySize{1000}
               << MdEntryType{'1'}
               << MdEntryPx{offer}
               << MdEntrySize{1000}
               << MdEntryType{'1'}
               << MdEntryPx{offer + 1}
               << MdEntrySize{2000};
            // clang-format on
        };
        conn_->send(now, "W"sv, fn);
    }
    void on_status(CyclTime now, Timer& tmr)
    {
        assert(conn_);
        SWIRLY_INFO << conn_->sess_id() << " <Ping> on_status";

        if (count_++ < 10) {
            auto fn = [](CyclTime now, ostream& os) {
                os << TradingSessionId::View{"OPEN"} << TradSesStatus{2};
            };
            conn_->send(now, "h"sv, fn);
        } else {
            conn_->logout(now);
        }
    }
    Reactor& reactor_;
    FixConn* conn_{nullptr};
    Timer md_tmr_, stat_tmr_;
    int count_{0};
    RandomBbo bbo_;
};

class PongHandler : public FixHandler {
  public:
    PongHandler(CyclTime now, Reactor& r, const FixSessId& sess_id, const Config& config) {}
    ~PongHandler() override = default;

    // Copy.
    PongHandler(const PongHandler&) = delete;
    PongHandler& operator=(const PongHandler&) = delete;

    // Move.
    PongHandler(PongHandler&&) = delete;
    PongHandler& operator=(PongHandler&&) = delete;

  protected:
    void do_on_logon(CyclTime now, FixConn& conn, const FixSessId& sess_id) override
    {
        SWIRLY_INFO << sess_id << " <Pong> on_logon";
        conn_ = &conn;
    }
    void do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                      Disconnect disconnect) noexcept override
    {
        if (disconnect == Disconnect::No) {
            SWIRLY_INFO << sess_id << " <Pong> on_logout";
        } else {
            SWIRLY_WARNING << sess_id << " <Pong> on_logout";
        }
        conn_ = nullptr;
    }
    void do_on_message(CyclTime now, FixConn& conn, string_view msg, size_t body_off, Version ver,
                       const FixHeader& hdr) override
    {
        SWIRLY_INFO << conn.sess_id() << " <Pong> on_message: " << hdr.msg_type.value;
    }
    void do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept override
    {
        SWIRLY_ERROR << conn.sess_id() << " <Pong> on_error: " << e.what();
    }
    void do_on_timeout(CyclTime now, const FixConn& conn) noexcept override
    {
        SWIRLY_WARNING << conn.sess_id() << " <Pong> on_timeout";
    }
    void do_prepare(CyclTime now, const FixSessId& sess_id,
                    const FixHandlerMap& handler_map) override
    {
        SWIRLY_INFO << sess_id << " <Pong> prepare";
    }
    void do_send(CyclTime now, string_view msg_type, string_view msg) override {}

  private:
    FixConn* conn_{nullptr};
};

class ProxyHandler : public FixHandler {
  public:
    ProxyHandler(CyclTime now, Reactor& r, const FixSessId& sess_id, const Config& config)
    : proxy_id_{sess_id.version, sess_id.sender_comp_id, config.get("proxy_comp_id")}
    {
    }
    ~ProxyHandler() override = default;

    // Copy.
    ProxyHandler(const ProxyHandler&) = delete;
    ProxyHandler& operator=(const ProxyHandler&) = delete;

    // Move.
    ProxyHandler(ProxyHandler&&) = delete;
    ProxyHandler& operator=(ProxyHandler&&) = delete;

  protected:
    void do_on_logon(CyclTime now, FixConn& conn, const FixSessId& sess_id) override
    {
        SWIRLY_INFO << sess_id << " <Proxy> on_logon";
        conn_ = &conn;
    }
    void do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                      Disconnect disconnect) noexcept override
    {
        if (disconnect == Disconnect::No) {
            SWIRLY_INFO << sess_id << " <Proxy> on_logout";
        } else {
            SWIRLY_WARNING << sess_id << " <Proxy> on_logout";
        }
        conn_ = nullptr;
    }
    void do_on_message(CyclTime now, FixConn& conn, string_view msg, size_t body_off, Version ver,
                       const FixHeader& hdr) override
    {
        SWIRLY_INFO << conn.sess_id() << " <Proxy> on_message: " << hdr.msg_type.value;
        if (hdr.msg_type.value == "W") {
            assert(proxy_);
            msg.remove_prefix(body_off);
            msg.remove_suffix(CheckSumLen);
            proxy_->send(now, hdr.msg_type.value, msg);
        }
    }
    void do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept override
    {
        SWIRLY_ERROR << conn.sess_id() << " <Proxy> on_error: " << e.what();
    }
    void do_on_timeout(CyclTime now, const FixConn& conn) noexcept override
    {
        SWIRLY_WARNING << conn.sess_id() << " <Proxy> on_timeout";
    }
    void do_prepare(CyclTime now, const FixSessId& sess_id,
                    const FixHandlerMap& handler_map) override
    {
        SWIRLY_INFO << sess_id << " <Proxy> prepare";
        proxy_ = handler_map.at(proxy_id_).get();
    }
    void do_send(CyclTime now, string_view msg_type, string_view msg) override
    {
        if (conn_) {
            auto fn = [msg](CyclTime now, ostream& os) { os << msg; };
            conn_->send(now, msg_type, fn);
        }
    }

  private:
    const FixSessId proxy_id_;
    FixHandler* proxy_{nullptr};
    FixConn* conn_{nullptr};
};

FixHandlerPtr make_handler(CyclTime now, const std::string& name, Reactor& r,
                           const FixSessId& sess_id, const Config& config)
{
    FixHandlerPtr h;
    if (name == "Ping") {
        h = make_unique<PingHandler>(now, r, sess_id, config);
    } else if (name == "Pong") {
        h = make_unique<PongHandler>(now, r, sess_id, config);
    } else if (name == "Proxy") {
        h = make_unique<ProxyHandler>(now, r, sess_id, config);
    } else {
        throw runtime_error{"invalid handler: " + name};
    }
    return h;
}

class FixApp : public FixAppBase {
  public:
    explicit FixApp(Reactor& r)
    : reactor_(r)
    {
    }
    ~FixApp() override = default;

    // Copy.
    FixApp(const FixApp&) = delete;
    FixApp& operator=(const FixApp&) = delete;

    // Move.
    FixApp(FixApp&&) = delete;
    FixApp& operator=(FixApp&&) = delete;

  protected:
    // FixHandler.

    void do_on_logon(CyclTime now, FixConn& conn, const FixSessId& sess_id) override
    {
        assert(!sess_id.empty());
        assert(handler_map_.count(sess_id));
        handler_map_[sess_id]->on_logon(now, conn, sess_id);
    }
    void do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                      Disconnect disconnect) noexcept override
    {
        assert(!sess_id.empty());
        assert(handler_map_.count(sess_id));
        handler_map_[sess_id]->on_logout(now, conn, sess_id, disconnect);
    }
    void do_on_message(CyclTime now, FixConn& conn, string_view msg, size_t body_off, Version ver,
                       const FixHeader& hdr) override
    {
        FixLexer lex{msg};
        while (!lex.empty()) {
            const auto [t, v] = lex.next();
            cout << t << '=' << v << '^';
        }
        cout << endl;
        const auto& sess_id = conn.sess_id();
        if (!sess_id.empty()) {
            assert(handler_map_.count(sess_id));
            handler_map_[sess_id]->on_message(now, conn, msg, body_off, ver, hdr);
        }
    }
    void do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept override
    {
        const auto& sess_id = conn.sess_id();
        if (!sess_id.empty()) {
            assert(handler_map_.count(sess_id));
            handler_map_[sess_id]->on_error(now, conn, e);
        } else {
            SWIRLY_ERROR << conn.endpoint() << " on_error: " << e.what();
        }
    }
    void do_on_timeout(CyclTime now, const FixConn& conn) noexcept override
    {
        const auto& sess_id = conn.sess_id();
        if (!sess_id.empty()) {
            assert(handler_map_.count(sess_id));
            handler_map_[sess_id]->on_timeout(now, conn);
        } else {
            SWIRLY_WARNING << conn.endpoint() << " on_timeout";
        }
    }

    // FixApp.

    void do_config(CyclTime now, const FixSessId& sess_id, const Config& config) override
    {
        SWIRLY_INFO << sess_id << " on_config";
        assert(!sess_id.empty());
        handler_map_[sess_id] = make_handler(now, config.get("handler"), reactor_, sess_id, config);
    }
    void do_prepare(CyclTime now) override
    {
        for (auto& [sess_id, handler] : handler_map_) {
            handler->prepare(now, sess_id, handler_map_);
        }
    }
    void do_on_connect(CyclTime now, FixConn& conn) override
    {
        SWIRLY_INFO << conn.endpoint() << " on_connect";
    }
    void do_on_disconnect(CyclTime now, const FixConn& conn) noexcept override
    {
        SWIRLY_INFO << conn.endpoint() << " on_disconnect";
    }

  private:
    Reactor& reactor_;
    FixHandlerMap handler_map_;
};

constexpr char ConfigData[] = R"(
version=4.4
heart_bt_int=5

[session]
type=initiator
endpoint=127.0.0.1:5002
sender_comp_id=CLIENT1
target_comp_id=SERVER1
handler=Ping

[session]
type=initiator
endpoint=127.0.0.1:5002
sender_comp_id=CLIENT2
target_comp_id=SERVER1
handler=Pong

[session]
type=acceptor
endpoint=0.0.0.0:5002
sender_comp_id=SERVER1
target_comp_id=CLIENT1
handler=Proxy
proxy_comp_id=CLIENT2

[session]
type=acceptor
endpoint=0.0.0.0:5002
sender_comp_id=SERVER1
target_comp_id=CLIENT2
handler=Proxy
proxy_comp_id=CLIENT1
)";

} // namespace

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        const auto start_time = CyclTime::set();

        EpollReactor reactor{1024};

        istringstream is{ConfigData};
        FixApp app{reactor};
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
