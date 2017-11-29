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
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

#include <swirly/web/HttpHandler.hpp>
#include <swirly/web/RestBody.hpp>
#include <swirly/web/Url.hpp>

#include <swirly/util/Log.hpp>
#include <swirly/util/RingBuffer.hpp>

#include <swirly/sys/TcpAcceptor.hpp>

#include <csignal>
#include <iostream>

namespace swirly {
} // namespace swirly

using namespace std;
using namespace swirly;
using namespace sys;

namespace {

class HttpSession
  : public EventHandler
  , public BasicHttpHandler<HttpSession> {

    friend class BasicHttpHandler<HttpSession>;
    using IntrusivePtr = boost::intrusive_ptr<HttpSession>;

  public:
    HttpSession(Reactor& r, IoSocket&& sock, const TcpEndpoint& ep, Time now)
      : EventHandler{r}
      , BasicHttpHandler<HttpSession>{HttpType::Request}
      , sock_{move(sock)}
    {
        SWIRLY_INFO(logMsg() << "start session");
        IntrusivePtr ptr{this, false};
        tok_ = r.subscribe(*sock_, Reactor::In, ptr);
        tmr_ = r.setTimer(now + 1s, 1s, ptr);
    }
    ~HttpSession() noexcept override = default;

  protected:
    void doClose() noexcept override;
    void doEvent(const MsgEvent& ev, Time now) override {}
    void doReady(int fd, FileEvents events, Time now) override
    {
        try {
            if (events & Reactor::Out) {
            }
            if (events & Reactor::In) {
                char data[256];
                const auto size = sys::read(fd, data, sizeof(data));
                if (size == 0) {
                    close();
                } else {
                    parse({data, size});
                }
            }
        } catch (const std::exception& e) {
            SWIRLY_ERROR(logMsg() << "exception handling url: " << e.what());
            pause();
            close();
        }
    }
    void doSignal(int sig, Time now) override { cout << "Signal: " << sig << endl; }
    void doTimer(const Timer& tmr, Time now) override { cout << "Timeout: " << now << endl; }

    bool onMessageBegin() noexcept { return true; }
    bool onUrl(std::string_view sv) noexcept;
    bool onStatus(std::string_view sv) noexcept
    {
        // Only supported for HTTP responses.
        return false;
    }
    bool onHeaderField(std::string_view sv, bool first) noexcept;
    bool onHeaderValue(std::string_view sv, bool first) noexcept;
    bool onHeadersEnd() noexcept;
    bool onBody(std::string_view sv) noexcept;
    bool onMessageEnd() noexcept;
    bool onChunkHeader(size_t len) noexcept { return true; }
    bool onChunkEnd() noexcept { return true; }

  private:
    IoSocket sock_;
    FileToken tok_;
    Timer tmr_;
    HttpRequest req_;
    RingBuffer<std::string> outbuf_{8};
};

void HttpSession::doClose() noexcept
{
    SWIRLY_INFO(logMsg() << "closing session");
    tmr_.reset();
    tok_.reset();
}

bool HttpSession::onUrl(string_view sv) noexcept
{
    bool ret{false};
    try {
        req_.appendUrl(sv);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling url: " << e.what());
        pause();
        close();
    }
    return ret;
}

bool HttpSession::onHeaderField(string_view sv, bool first) noexcept
{
    bool ret{false};
    try {
        req_.appendHeaderField(sv, first);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling header field: " << e.what());
        pause();
        close();
    }
    return ret;
}

bool HttpSession::onHeaderValue(string_view sv, bool first) noexcept
{
    bool ret{false};
    try {
        req_.appendHeaderValue(sv, first);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling header value: " << e.what());
        pause();
        close();
    }
    return ret;
}

bool HttpSession::onHeadersEnd() noexcept
{
    req_.setMethod(method());
    return true;
}

bool HttpSession::onBody(string_view sv) noexcept
{
    bool ret{false};
    try {
        req_.appendBody(sv);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling body: " << e.what());
        pause();
        close();
    }
    return ret;
}

bool HttpSession::onMessageEnd() noexcept
{
    bool ret{false};
    try {
        req_.flush(); // May throw.
        const auto wasEmpty = outbuf_.empty();
        outbuf_.post([](auto& ref) { ref.clear(); });
        {
            HttpResponse resp{outbuf_.back()};
            resp << "{\"msg\":\"Hello, World!\"}";
        }
        if (outbuf_.full()) {
            // Interrupt parser if output buffer is full.
            pause();
        }
        if (wasEmpty) {
            // May throw.
            reactor().setMask(*sock_, Reactor::In | Reactor::Out);
        }
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling message: " << e.what());
        pause();
        close();
    }
    req_.clear();
    return ret;
}

class HttpAcceptor : public TcpAcceptor {
  public:
    using TcpAcceptor::TcpAcceptor;
    ~HttpAcceptor() noexcept override { cout << "~HttpAcceptor()\n"; }

  protected:
    void doClose() noexcept override {}
    void doAccept(IoSocket&& sock, const Endpoint& ep, Time now) override
    {
        cout << "doAccept: " << ep << endl;
        new HttpSession{reactor(), move(sock), ep, now};
    }

  private:
};

volatile sig_atomic_t quit{0};

} // namespace

int main(int argc, char* argv[])
{
    signal(SIGINT, [](int) { ++quit; });

    int ret = 1;
    try {

        Reactor r{1024};
        const auto ep = parseEndpoint<Tcp>("0.0.0.0:8080");
        new HttpAcceptor{r, ep};
        while (!quit) {
            r.poll();
        }
        ret = 0;
    } catch (const exception& e) {
        SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
    return ret;
}
