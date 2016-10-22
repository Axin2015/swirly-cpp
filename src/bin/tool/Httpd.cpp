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

#include <swirly/fir/HttpHandler.hpp>
#include <swirly/fir/Url.hpp>

#include <swirly/ash/Log.hpp>
#include <swirly/ash/RefCounted.hpp>
#include <swirly/ash/RingBuffer.hpp>
#include <swirly/ash/String.hpp>

#include <boost/asio.hpp>

#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <utility>

using namespace boost;
using namespace std;
using namespace swirly;

using asio::ip::tcp;

class Session;
using SessionPtr = intrusive_ptr<Session>;

class Session : public RefCounted<Session>, //
                public BasicHttpHandler<Session>, //
                public BasicUrl<Session> {

  friend class BasicHttpHandler<Session>;
  enum { IdleTimeout = 5, MaxData = 4096 };

 public:
  explicit Session(asio::io_service& service)
    : BasicHttpHandler<Session>{HttpType::Request}, socket_{service}, timeout_(service)
  {
  }
  const auto& url() const noexcept { return url_; }
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
  void clear() noexcept
  {
    url_.clear();
    status_.clear();
    headers_.clear();
    body_.clear();
  }
  void consume()
  {
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
  void produce(string_view data)
  {
    const auto wasEmpty = output_.empty();
    output_.write([data](auto& ref) { ref.assign(data.data(), data.size()); });
    if (wasEmpty) {
      writeNext();
    }
  }
  using BasicHttpHandler<Session>::parse;
  void readSome()
  {
    SessionPtr session{this};
    socket_.async_read_some(asio::buffer(data_, MaxData), [this, session](auto ec, auto len) {
      if (!ec) {
        input_ = asio::buffer(data_, len);
        this->consume();
        this->resetTimeout();
      } else if (ec == asio::error::eof) {
      } else if (ec != asio::error::operation_aborted) {
        SWIRLY_INFO("read cancelled");
      }
    });
  }
  void writeNext()
  {
    const auto& data = output_.front();
    SessionPtr session{this};
    asio::async_write(socket_, asio::buffer(data), [this, session](auto ec, auto len) {
      if (!ec) {
        const auto wasFull = output_.full();
        this->output_.pop();
        if (wasFull) {
          this->consume();
        }
        if (!this->output_.empty()) {
          this->writeNext();
        }
      } else if (ec != asio::error::operation_aborted) {
        SWIRLY_WARNING("write cancelled");
      }
    });
  }
  void resetTimeout()
  {
    // Timeout timer fires when client is idle. The client is expected to send regular heartbeats to
    // prevent this.
    system::error_code e;
    timeout_.cancel(e);
    timeout_.expires_from_now(posix_time::seconds{IdleTimeout});

    SessionPtr session{this};
    timeout_.async_wait([this, session](auto ec) {
      if (!ec) {
        this->stop();
      } else if (ec != asio::error::operation_aborted) {
        SWIRLY_INFO("timer cancelled");
      }
    });
  }
  bool onMessageBegin() noexcept { return true; }
  bool onUrl(string_view sv) noexcept
  {
    url_.append(sv.data(), sv.size());
    return true;
  }
  bool onStatus(string_view sv) noexcept
  {
    status_.append(sv.data(), sv.size());
    return true;
  }
  bool onHeaderField(string_view sv, bool create) noexcept
  {
    if (create) {
      headers_.emplace_back(string{sv.data(), sv.size()}, "");
    } else {
      headers_.back().first.append(sv.data(), sv.size());
    }
    return true;
  }
  bool onHeaderValue(string_view sv) noexcept
  {
    headers_.back().second.append(sv.data(), sv.size());
    return true;
  }
  bool onHeadersEnd() noexcept { return true; }
  bool onBody(string_view sv) noexcept
  {
    body_.append(sv.data(), sv.size());
    return true;
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

    BasicUrl<Session>::parse();
    produce(Message);
    if (output_.full()) {
      // Interrupt parser if output buffer is full.
      pause();
    }
    // Reset HTTP message content.
    BasicUrl<Session>::reset();
    clear();
    return true;
  }
  bool onChunkHeader(size_t len) noexcept { return true; }
  bool onChunkEnd() noexcept { return true; }

  string url_;
  string status_;
  vector<pair<string, string>> headers_;
  string body_;

  tcp::socket socket_;
  // Close session if client is inactive.
  asio::deadline_timer timeout_;
  char data_[MaxData];
  boost::asio::const_buffer input_;
  RingBuffer<string> output_{8};
};

class Server {
 public:
  Server(asio::io_service& service, std::uint16_t port) : service_{service}, acceptor_{service}
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
    auto session = makeRefCounted<Session>(service_);
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
    asio::io_service service;
    Server server{service, static_cast<uint16_t>(atoi(argv[1]))};
    service.run();
    ret = 0;
  } catch (const std::exception& e) {
    cerr << "exception: " << e.what() << endl;
  }
  return ret;
}
