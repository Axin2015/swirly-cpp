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
#ifndef SWIRLY_HTTP_CONN_HPP
#define SWIRLY_HTTP_CONN_HPP

#include <swirly/http/Parser.hpp>
#include <swirly/http/Request.hpp>
#include <swirly/http/Stream.hpp>

#include <swirly/app/MemAlloc.hpp>

#include <swirly/sys/Event.hpp>
#include <swirly/sys/IoSocket.hpp>
#include <swirly/sys/IpAddress.hpp>
#include <swirly/sys/Reactor.hpp>

#include <boost/intrusive/list.hpp>

namespace swirly {
inline namespace http {
class HttpApp;

template <typename RequestT, typename AppT>
class BasicHttpConn
: public MemAlloc
, BasicHttpParser<BasicHttpConn<RequestT, AppT>> {

    friend class BasicHttpParser<BasicHttpConn<RequestT, AppT>>;

    using Request = RequestT;
    using App = AppT;
    // Automatically unlink when object is destroyed.
    using AutoUnlinkOption = boost::intrusive::link_mode<boost::intrusive::auto_unlink>;

    static constexpr auto IdleTimeout = 5s;

    using BasicHttpParser<BasicHttpConn<RequestT, AppT>>::method;
    using BasicHttpParser<BasicHttpConn<RequestT, AppT>>::parse;
    using BasicHttpParser<BasicHttpConn<RequestT, AppT>>::should_keep_alive;

  public:
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    BasicHttpConn(Time now, Reactor& r, IoSocket&& sock, const Endpoint& ep, App& app)
    : BasicHttpParser<BasicHttpConn<RequestT, AppT>>{HttpType::Request}
    , reactor_(r)
    , sock_{std::move(sock)}
    , ep_{ep}
    , app_(app)
    {
        sub_ = r.subscribe(*sock_, EventIn, bind<&BasicHttpConn::on_io_event>(this));
        tmr_ = r.timer(now + IdleTimeout, Priority::Low, bind<&BasicHttpConn::on_timer>(this));
        app.on_connect(now, ep_);
    }

    // Copy.
    BasicHttpConn(const BasicHttpConn&) = delete;
    BasicHttpConn& operator=(const BasicHttpConn&) = delete;

    // Move.
    BasicHttpConn(BasicHttpConn&&) = delete;
    BasicHttpConn& operator=(BasicHttpConn&&) = delete;

    const Endpoint& endpoint() const noexcept { return ep_; }
    void clear() noexcept { req_.clear(); }
    void dispose(Time now) noexcept
    {
        app_.on_disconnect(now, ep_);
        delete this;
    }

    boost::intrusive::list_member_hook<AutoUnlinkOption> list_hook;

  private:
    ~BasicHttpConn() = default;
    bool on_message_begin() noexcept
    {
        ++pending_;
        return true;
    }
    bool on_url(std::string_view sv) noexcept
    {
        bool ret{false};
        try {
            req_.append_url(sv);
            ret = true;
        } catch (const std::exception& e) {
            app_.on_error(now_, ep_, e);
        }
        return ret;
    }
    bool on_status(std::string_view sv) noexcept
    {
        // Only supported for HTTP responses.
        return false;
    }
    bool on_header_field(std::string_view sv, bool first) noexcept
    {
        bool ret{false};
        try {
            req_.append_header_field(sv, first);
            ret = true;
        } catch (const std::exception& e) {
            app_.on_error(now_, ep_, e);
        }
        return ret;
    }
    bool on_header_value(std::string_view sv, bool first) noexcept
    {
        bool ret{false};
        try {
            req_.append_header_value(sv, first);
            ret = true;
        } catch (const std::exception& e) {
            app_.on_error(now_, ep_, e);
        }
        return ret;
    }
    bool on_headers_end() noexcept
    {
        req_.set_method(method());
        return true;
    }
    bool on_body(std::string_view sv) noexcept
    {
        bool ret{false};
        try {
            req_.append_body(sv);
            ret = true;
        } catch (const std::exception& e) {
            app_.on_error(now_, ep_, e);
        }
        return ret;
    }
    bool on_message_end() noexcept
    {
        bool ret{false};
        try {
            --pending_;
            req_.flush(); // May throw.

            const auto was_empty = out_.empty();
            app_.on_message(now_, ep_, req_, os_);

            if (was_empty) {
                // May throw.
                sub_.set_events(EventIn | EventOut);
            }
            ret = true;
        } catch (const std::exception& e) {
            app_.on_error(now_, ep_, e);
        }
        req_.clear();
        return ret;
    }
    bool on_chunk_header(std::size_t len) noexcept { return true; }
    bool on_chunk_end() noexcept { return true; }
    void on_io_event(Time now, int fd, unsigned events)
    {
        try {
            if (events & EventOut) {
                out_.consume(os::write(fd, out_.data()));
                if (out_.empty()) {
                    if (should_keep_alive()) {
                        // May throw.
                        sub_.set_events(EventIn);
                    } else {
                        dispose(now);
                    }
                }
            }
            if (events & (EventIn | EventHup)) {
                const auto size = os::read(fd, in_.prepare(2048));
                if (size > 0) {
                    // Commit actual bytes read.
                    in_.commit(size);

                    now_ = now;
                    in_.consume(parse(in_.data()));

                    // Reset timer.
                    tmr_.cancel();
                    tmr_ = reactor_.timer(now + IdleTimeout, Priority::Low,
                                          bind<&BasicHttpConn::on_timer>(this));
                } else {
                    dispose(now);
                }
            }
        } catch (const ProtocolException&) {
            // Do not call on_error() here, because it will have already been called in one of the
            // noexcept parser callback functions.
            dispose(now);
        } catch (const std::exception& e) {
            app_.on_error(now_, ep_, e);
            dispose(now);
        }
    }
    void on_timer(Time now, Timer& tmr)
    {
        app_.on_timeout(now, ep_);
        dispose(now);
    }

    Reactor& reactor_;
    IoSocket sock_;
    Endpoint ep_;
    App& app_;
    Reactor::Handle sub_;
    Timer tmr_;
    Time now_{};
    int pending_{0};
    Buffer in_, out_;
    Request req_;
    HttpStream os_{out_};
};

using HttpConn = BasicHttpConn<HttpRequest, HttpApp>;

} // namespace http
} // namespace swirly

#endif // SWIRLY_HTTP_CONN_HPP
