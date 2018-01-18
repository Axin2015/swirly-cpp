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
#include "HttpSess.hpp"

#include "HttpResponse.hpp"
#include "RestServ.hpp"

#include <swirly/sys/MemAlloc.hpp>

using namespace boost;
using namespace std;

namespace swirly {
namespace {

template <typename FnT>
class AllocHandler {
  public:
    explicit AllocHandler(FnT fn) noexcept
      : fn_{move(fn)}
    {
    }
    ~AllocHandler() noexcept = default;

    // Copy.
    AllocHandler(const AllocHandler&) = default;
    AllocHandler& operator=(const AllocHandler&) = default;

    // Move.
    AllocHandler(AllocHandler&&) = default;
    AllocHandler& operator=(AllocHandler&&) = default;

    template <typename... ArgsT>
    void operator()(ArgsT&&... args)
    {
        fn_(forward<ArgsT>(args)...);
    }
    friend void* asio_handler_allocate(size_t size, AllocHandler<FnT>*) { return alloc(size); }
    friend void asio_handler_deallocate(void* pointer, size_t size, AllocHandler<FnT>*) noexcept
    {
        dealloc(pointer, size);
    }

  private:
    FnT fn_;
};

template <typename FnT>
auto makeAllocHandler(FnT fn)
{
    return AllocHandler<FnT>{move(fn)};
}

} // namespace

HttpSess::~HttpSess() noexcept = default;

void HttpSess::start()
{
    SWIRLY_INFO(logMsg() << "start session");
    asyncReadSome();
    resetTimeout();
}

void HttpSess::stop() noexcept
{
    SWIRLY_INFO(logMsg() << "stop session");

    system::error_code ec;
    // Any asynchronous send, receive or connect operations will be cancelled immediately, and will
    // complete with the asio::error::operation_aborted error.
    sock_.close(ec);
    // Cancelling timers should result in a zero reference count and destruction.
    timeout_.cancel(ec);
}

void HttpSess::parse()
{
    const auto size = asio::buffer_size(inbuf_);
    if (size > 0) {
        const auto* data = asio::buffer_cast<const char*>(inbuf_);
        const auto n = BasicHttpHandler::parse({data, size});
        // Slide buffer window forward based on number of bytes consumed by parser.
        inbuf_ = asio::buffer(data + n, size - n);
    }
    // Continue reading if the output buffer has more space.
    if (!outbuf_.full()) {
        asyncReadSome();
    }
}

void HttpSess::resetTimeout()
{
    // Timeout timer fires when client is idle. The client is expected to send regular heartbeats to
    // prevent this.
    system::error_code ec;
    timeout_.cancel(ec);
    timeout_.expires_from_now(posix_time::seconds{IdleTimeout});

    HttpSessPtr session{this};
    timeout_.async_wait([this, session](auto ec) {
        if (!ec) {
            this->stop();
        } else if (ec == asio::error::operation_aborted) {
            SWIRLY_DEBUG(this->logMsg() << "timer cancelled");
        } else {
            SWIRLY_ERROR(this->logMsg() << "exception on async timeout: " << ec);
            this->stop();
        }
    });
}

void HttpSess::asyncReadSome()
{
    HttpSessPtr session{this};
    auto fn = [this, session](auto ec, auto len) {
        if (!ec) {
            this->onReadSome(len);
        } else if (ec == asio::error::operation_aborted) {
            SWIRLY_INFO(this->logMsg() << "read cancelled");
        } else {
            if (ec != asio::error::eof) {
                SWIRLY_ERROR(this->logMsg() << "exception on async read: " << ec);
            }
            this->stop();
        }
    };
    sock_.async_read_some(asio::buffer(data_, MaxData), makeAllocHandler(fn));
}

void HttpSess::asyncWrite()
{
    const auto& data = outbuf_.front();
    HttpSessPtr session{this};
    auto fn = [this, session](auto ec, auto len) {
        if (!ec) {
            this->onWrite();
        } else if (ec == asio::error::operation_aborted) {
            SWIRLY_WARNING(this->logMsg() << "write cancelled");
        } else {
            SWIRLY_ERROR(this->logMsg() << "exception on async write: " << ec);
            this->stop();
        }
    };
    asio::async_write(sock_, asio::buffer(data), makeAllocHandler(fn));
}

void HttpSess::onReadSome(size_t len) noexcept
{
    inbuf_ = asio::buffer(data_, len);
    try {
        parse();
        resetTimeout();
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling read: " << e.what());
        stop();
    }
}

void HttpSess::onWrite() noexcept
{
    const auto wasFull = outbuf_.full();
    outbuf_.pop();
    try {
        if (!outbuf_.empty()) {
            asyncWrite();
        }
        if (wasFull) {
            parse();
        }
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling write: " << e.what());
        stop();
    }
}

bool HttpSess::onUrl(string_view sv) noexcept
{
    bool ret{false};
    try {
        req_.appendUrl(sv);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling url: " << e.what());
        pause();
        stop();
    }
    return ret;
}

bool HttpSess::onHeaderField(string_view sv, bool first) noexcept
{
    bool ret{false};
    try {
        req_.appendHeaderField(sv, first);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling header field: " << e.what());
        pause();
        stop();
    }
    return ret;
}

bool HttpSess::onHeaderValue(string_view sv, bool first) noexcept
{
    bool ret{false};
    try {
        req_.appendHeaderValue(sv, first);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling header value: " << e.what());
        pause();
        stop();
    }
    return ret;
}

bool HttpSess::onHeadersEnd() noexcept
{
    req_.setMethod(method());
    return true;
}

bool HttpSess::onBody(string_view sv) noexcept
{
    bool ret{false};
    try {
        req_.appendBody(sv);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling body: " << e.what());
        pause();
        stop();
    }
    return ret;
}

bool HttpSess::onMessageEnd() noexcept
{
    bool ret{false};
    try {
        req_.flush(); // May throw.
        const auto wasEmpty = outbuf_.empty();
        outbuf_.post([](auto& ref) { ref.clear(); });
        {
            HttpResponse resp{outbuf_.back()};
            restServ_.handleRequest(req_, resp);
        }
        if (outbuf_.full()) {
            // Interrupt parser if output buffer is full.
            pause();
        }
        if (wasEmpty) {
            asyncWrite(); // May throw.
        }
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling message: " << e.what());
        pause();
        stop();
    }
    req_.clear();
    return ret;
}

} // namespace swirly
