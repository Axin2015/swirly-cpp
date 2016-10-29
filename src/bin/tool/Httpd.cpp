/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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

// Experimental replacement for Mongoose. This is a work in progress.

#include <swirly/ws/HttpHandler.hpp>
#include <swirly/ws/RestRequest.hpp>
#include <swirly/ws/Url.hpp>

#include <swirly/util/Log.hpp>
#include <swirly/util/MemPool.hpp>
#include <swirly/util/RefCounted.hpp>
#include <swirly/util/RingBuffer.hpp>
#include <swirly/util/String.hpp>

#define BOOST_ASIO_DISABLE_THREADS 1
#include <boost/asio.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

using namespace boost;
using namespace swirly;

using asio::ip::tcp;

namespace {

MemPool memPool;

} // anonymous

namespace swirly {

void* alloc(size_t size)
{
  return memPool.alloc(size);
}

void dealloc(void* ptr, size_t size) noexcept
{
  return memPool.dealloc(ptr, size);
}

} // swirly

template <typename FnT>
class AllocHandler {
 public:
  explicit AllocHandler(FnT fn) noexcept : fn_{std::move(fn)} {}
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
    fn_(std::forward<ArgsT>(args)...);
  }
  friend void* asio_handler_allocate(std::size_t size, AllocHandler<FnT>*) { return alloc(size); }
  friend void asio_handler_deallocate(void* pointer, std::size_t size, AllocHandler<FnT>*) noexcept
  {
    dealloc(pointer, size);
  }

 private:
  FnT fn_;
};

template <typename FnT>
auto makeAllocHandler(FnT fn)
{
  return AllocHandler<FnT>{std::move(fn)};
}

class HttpSession;
using HttpSessionPtr = intrusive_ptr<HttpSession>;

class HttpMessage : public BasicUrl<HttpMessage> {
 public:
  const auto& url() const noexcept { return url_; }
  void clear() noexcept
  {
    BasicUrl<HttpMessage>::reset();
    url_.clear();
    headerField_.clear();
    headerValue_.clear();
    request_.reset();
  }
  void flush() { BasicUrl<HttpMessage>::parse(); }
  void appendUrl(std::string_view sv) { url_ += sv; }
  void appendHeaderField(std::string_view sv, bool create)
  {
    if (create) {
      headerField_ = sv;
      headerValue_.clear();
    } else {
      headerValue_ += sv;
    }
  }
  void appendHeaderValue(std::string_view sv) { headerValue_ += sv; }
  void appendBody(std::string_view sv) { request_.parse(sv); }
 private:
  // Header fields:
  // Swirly-Time
  // Swirly-Accnt
  // Swirly-Perm
  String<128> url_;
  String<16> headerField_;
  String<24> headerValue_;
  RestRequest request_;
};

class HttpSession : public RefCounted<HttpSession>, public BasicHttpHandler<HttpSession> {

  friend class BasicHttpHandler<HttpSession>;
  enum { IdleTimeout = 5, MaxData = 4096 };

 public:
  explicit HttpSession(asio::io_service& service)
    : BasicHttpHandler<HttpSession>{HttpType::Request}, socket_{service}, timeout_(service)
  {
  }
  void start()
  {
    readSome();
    resetTimeout();
  }
  void stop()
  {
    system::error_code e;
    // Any asynchronous send, receive or connect operations will be cancelled immediately, and will
    // complete with the asio::error::operation_aborted error.
    socket_.close(e);
    // Cancelling timers should result in a zero reference count and destruction.
    timeout_.cancel(e);
  }
  tcp::socket& socket() { return socket_; }

 private:
  void consume()
  {
    SWIRLY_DEBUG("consume"_sv);
    // FIXME: close socket on parse exception.
    const auto size = asio::buffer_size(input_);
    if (size > 0) {
      const auto* data = asio::buffer_cast<const char*>(input_);
      const auto n = parse({data, size});
      // Slide buffer window forward based on number of bytes consumed by parser.
      input_ = asio::buffer(data + n, size - n);
    }
    // Continue reading if the output buffer has more space.
    if (!output_.full()) {
      readSome();
    }
  }
  using BasicHttpHandler<HttpSession>::parse;
  void readSome()
  {
    SWIRLY_DEBUG("readSome"_sv);
    HttpSessionPtr session{this};

    socket_.async_read_some(asio::buffer(data_, MaxData),
                            makeAllocHandler([this, session](auto ec, auto len) {
                              SWIRLY_DEBUG("async_read_some"_sv);
                              if (!ec) {
                                input_ = asio::buffer(data_, len);
                                this->consume();
                                this->resetTimeout();
                              } else if (ec == asio::error::eof) {
                              } else if (ec != asio::error::operation_aborted) {
                                SWIRLY_INFO("read cancelled"_sv);
                              }
                            }));
  }
  void write()
  {
    SWIRLY_DEBUG("write"_sv);
    const auto& data = output_.front();
    HttpSessionPtr session{this};
    asio::async_write(socket_, asio::buffer(data),
                      makeAllocHandler([this, session](auto ec, auto len) {
                        SWIRLY_DEBUG("async_write"_sv);
                        if (!ec) {
                          const auto wasFull = output_.full();
                          this->output_.pop();
                          if (!this->output_.empty()) {
                            this->write();
                          }
                          if (wasFull) {
                            this->consume();
                          }
                        } else if (ec != asio::error::operation_aborted) {
                          SWIRLY_WARNING("write cancelled"_sv);
                        }
                      }));
  }
  void resetTimeout()
  {
    // Timeout timer fires when client is idle. The client is expected to send regular heartbeats to
    // prevent this.
    system::error_code e;
    timeout_.cancel(e);
    timeout_.expires_from_now(posix_time::seconds{IdleTimeout});

    HttpSessionPtr session{this};
    timeout_.async_wait([this, session](auto ec) {
      if (!ec) {
        this->stop();
      } else if (ec != asio::error::operation_aborted) {
        SWIRLY_INFO("timer cancelled"_sv);
      }
    });
  }
  bool onMessageBegin() noexcept { return true; }
  bool onUrl(std::string_view sv) noexcept
  {
    bool ret{false};
    try {
      message_.appendUrl(sv);
      ret = true;
    } catch (const std::exception& e) {
      SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
    return ret;
  }
  bool onStatus(std::string_view sv) noexcept
  {
    // Only supported for HTTP responses.
    return false;
  }
  bool onHeaderField(std::string_view sv, bool create) noexcept
  {
    bool ret{false};
    try {
      message_.appendHeaderField(sv, create);
      ret = true;
    } catch (const std::exception& e) {
      SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
    return ret;
  }
  bool onHeaderValue(std::string_view sv) noexcept
  {
    bool ret{false};
    try {
      message_.appendHeaderValue(sv);
      ret = true;
    } catch (const std::exception& e) {
      SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
    return ret;
  }
  bool onHeadersEnd() noexcept { return true; }
  bool onBody(std::string_view sv) noexcept
  {
    bool ret{false};
    try {
      message_.appendBody(sv);
      ret = true;
    } catch (const std::exception& e) {
      SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
    return ret;
  }
  bool onMessageEnd() noexcept
  {
    constexpr auto Message = //
      "HTTP/1.1 200 OK\r\n" //
      "Connection: keep-alive\r\n" //
      "Content-Type: text/plain\r\n" //
      "Content-Length: 15\r\n" //
      "\r\n" //
      "Hello, World!\r\n"_sv;

    bool ret{false};
    try {
      message_.flush();
      const std::string_view data = Message;
      const auto wasEmpty = output_.empty();
      output_.write([data](auto& ref) { ref.assign(data.data(), data.size()); });
      if (wasEmpty) {
        write();
      }
      message_.clear();
      if (output_.full()) {
        // Interrupt parser if output buffer is full.
        pause();
      }
      ret = true;
    } catch (const std::exception& e) {
      SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
    return ret;
  }
  bool onChunkHeader(size_t len) noexcept { return true; }
  bool onChunkEnd() noexcept { return true; }

  tcp::socket socket_;
  // Close session if client is inactive.
  asio::deadline_timer timeout_;
  char data_[MaxData];
  boost::asio::const_buffer input_;
  HttpMessage message_;
  RingBuffer<std::string> output_{8};
};

class HttpServer {
 public:
  HttpServer(asio::io_service& service, std::uint16_t port) : service_{service}, acceptor_{service}
  {
    tcp::endpoint endpoint{asio::ip::tcp::v4(), port};
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address{true});
    acceptor_.bind(endpoint);
    acceptor_.listen();

    accept();
  }

 private:
  void accept()
  {
    auto session = makeRefCounted<HttpSession>(service_);
    acceptor_.async_accept(session->socket(), [this, session](auto ec) {
      if (!ec) {
        session->start();
      }
      this->accept();
    });
  }

  asio::io_service& service_;
  tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
  using namespace std;
  int ret{1};
  try {
    if (argc != 2) {
      cerr << "usage: swirly_server <port>\n";
      return 1;
    }
    memPool.reserve(1 << 20);
    asio::io_service service;
    HttpServer server{service, static_cast<uint16_t>(atoi(argv[1]))};
    service.run();
    ret = 0;
  } catch (const std::exception& e) {
    cerr << "exception: " << e.what() << endl;
  }
  return ret;
}
