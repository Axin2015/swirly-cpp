/*
 * Swirly Order-Book and Matching-Engine.
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
#include "Mock.hpp"
#include "Mongoose.hpp"
#include "Stream.hpp"

#include <swirly/fir/Rest.hpp>

#include <swirly/ash/Time.hpp>
#include <swirly/ash/Tokeniser.hpp>

#include <iostream>

using namespace std;
using namespace swirly;

namespace {

constexpr char HTTP_PORT[] = "8080";

static struct mg_serve_http_opts httpOpts;

class RestServ : public mg::Mgr<RestServ> {
 public:
  explicit RestServ(Rest& rest) noexcept : rest_(rest) {}
  void reset(StringView sv) noexcept
  {
    out_.reset();

    // Remove leading slash.
    if (sv.front() == '/')
      sv.remove_prefix(1);
    uri_.reset(sv);
  }
  void httpRequest(mg_connection& nc, mg::HttpMessage data)
  {
    reset(data.uri());

    if (!uri_.empty()) {

      const auto tok = uri_.top();
      uri_.pop();
      const auto method = data.method();

      if (tok == "rec") {
        if (method == "GET") {
          getRec(data);
        } else if (method == "POST") {
          postRec(data);
        } else if (method == "PUT") {
          putRec(data);
        } else {
          // FIXME.
        }
      } else if (tok == "sess") {
        if (method == "GET") {
          getSess(data);
        } else if (method == "POST") {
          postSess(data);
        } else if (method == "PUT") {
          putSess(data);
        } else if (method == "DELETE") {
          deleteSess(data);
        } else {
          // FIXME.
        }
      } else if (tok == "view") {
        if (method == "GET") {
          getView(data);
        } else {
          // FIXME.
        }
      } else {
        mg_serve_http(&nc, data.get(), httpOpts);
        return;
      }

      const auto len = static_cast<int>(out_.size());
      mg_printf(&nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%.*s", len, len, out_.data());
    }
  }
  void getRec(mg::HttpMessage data)
  {
    rest_.assets(now(), out_);
    out_ << R"({"mnem":"EURUSD"})";
  }
  void postRec(mg::HttpMessage data) {}
  void putRec(mg::HttpMessage data) {}
  void getSess(mg::HttpMessage data) {}
  void postSess(mg::HttpMessage data) {}
  void putSess(mg::HttpMessage data) {}
  void deleteSess(mg::HttpMessage data) {}
  void getView(mg::HttpMessage data) {}

 private:
  Rest& rest_;
  mg::OStream out_;
  Tokeniser<'/'> uri_;
};

} // anonymous

int main(int argc, char* argv[])
{
  try {

    MockModel model;
    MockJourn journ;
    Rest rest{model, journ, now()};

    RestServ rs{rest};
    auto& conn = rs.bind(HTTP_PORT);
    mg_set_protocol_http_websocket(&conn);

    httpOpts.document_root = ".";
    httpOpts.dav_document_root = ".";
    httpOpts.enable_directory_listing = "yes";

    cout << "Starting web server on port " << HTTP_PORT << endl;
    for (;;) {
      rs.poll(1000);
    }

    return 0;
  } catch (const exception& e) {
    cerr << "error: " << e.what() << endl;
  }
  return 1;
}
