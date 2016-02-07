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
#include "mongoose.h"

#include <swirly/ash/Exception.hpp>
#include <swirly/ash/Tokeniser.hpp>

#include <iostream>

namespace swirly {
namespace mg {

class StreamBuf : public std::streambuf {
    mbuf buf_;

 protected:
    int_type overflow(int_type c) noexcept override
    {
        if (c != traits_type::eof()) {
            const char z = c;
            if (mbuf_append(&buf_, &z, 1) != 1)
                c = traits_type::eof();
        }
        return c;
    }
    std::streamsize xsputn(const char_type* s, std::streamsize count) noexcept override
    {
        return mbuf_append(&buf_, s, count);
    }

 public:
    StreamBuf() noexcept
    {
        mbuf_init(&buf_, 4096);
    }
    ~StreamBuf() noexcept
    {
        mbuf_free(&buf_);
    }

    // Copy.
    StreamBuf(const StreamBuf& rhs) = delete;
    StreamBuf& operator=(const StreamBuf& rhs) = delete;

    // Move.
    StreamBuf(StreamBuf&&) = delete;
    StreamBuf& operator=(StreamBuf&&) = delete;

    void reset() noexcept
    {
        buf_.len = 0;
    }
    const char_type* data() const noexcept
    {
        return buf_.buf;
    }
    std::streamsize size() const noexcept
    {
        return buf_.len;
    }
};

class OStream : public std::ostream {
    StreamBuf buf_;

 public:
    OStream() : std::ostream{nullptr}
    {
        rdbuf(&buf_);
    }

    // Copy.
    OStream(const OStream& rhs) = delete;
    OStream& operator=(const OStream& rhs) = delete;

    // Move.
    OStream(OStream&&) = delete;
    OStream& operator=(OStream&&) = delete;

    void reset() noexcept
    {
        buf_.reset();
    }
    const char_type* data() const noexcept
    {
        return buf_.data();
    }
    std::streamsize size() const noexcept
    {
        return buf_.size();
    }
};

StringView operator+(const mg_str& str) noexcept
{
    return {str.p, str.len};
}

class Error : public Exception {
 public:
    Error() noexcept = default;

    ~Error() noexcept = default;

    // Copy.
    Error(const Error&) noexcept = default;
    Error& operator=(const Error&) noexcept = default;

    // Move.
    Error(Error&&) noexcept = default;
    Error& operator=(Error&&) noexcept = default;
};

class HttpMessage {
    http_message* impl_;

 public:
    HttpMessage(http_message* impl) noexcept : impl_{impl}
    {
    }
    ~HttpMessage() noexcept = default;

    // Copy.
    HttpMessage(const HttpMessage&) noexcept = default;
    HttpMessage& operator=(const HttpMessage&) noexcept = default;

    // Move.
    HttpMessage(HttpMessage&&) noexcept = default;
    HttpMessage& operator=(HttpMessage&&) noexcept = default;

    auto get() const noexcept
    {
        return impl_;
    }
    auto method() const noexcept
    {
        return +impl_->method;
    }
    auto uri() const noexcept
    {
        return +impl_->uri;
    }
    auto proto() const noexcept
    {
        return +impl_->proto;
    }
    auto queryString() const noexcept
    {
        return +impl_->query_string;
    }
    auto body() const noexcept
    {
        return +impl_->body;
    }
};

// Mongoose event manager.

template <typename DerivedT>
class MgrBase {
    mg_mgr mgr_;

    static void handler(mg_connection* conn, int event, void* data)
    {
        auto* self = static_cast<DerivedT*>(conn->user_data);
        switch (event) {
        case MG_EV_CLOSE:
            conn->user_data = nullptr;
            break;
        case MG_EV_HTTP_REQUEST:
            self->httpRequest(*conn, static_cast<http_message*>(data));
            break;
        }
    }

 protected:
    MgrBase() noexcept
    {
        mg_mgr_init(&mgr_, this);
    }
    ~MgrBase() noexcept
    {
        mg_mgr_free(&mgr_);
    }

 public:
    // Copy.
    MgrBase(const MgrBase&) = delete;
    MgrBase& operator=(const MgrBase&) = delete;

    // Move.
    MgrBase(MgrBase&&) = delete;
    MgrBase& operator=(MgrBase&&) = delete;

    mg_connection& bind(const char* addr)
    {
        auto* conn = mg_bind(&mgr_, addr, handler);
        if (!conn)
            throwException<Error>("mg_bind() failed");
        conn->user_data = this;
        return *conn;
    }
    time_t poll(int milli)
    {
        return mg_mgr_poll(&mgr_, milli);
    }
};

} // mg
} // swirly

using namespace std;
using namespace swirly;

namespace {

constexpr char HTTP_PORT[] = "8080";

static struct mg_serve_http_opts httpOpts;

class Mgr : public mg::MgrBase<Mgr> {
    mg::OStream os_;
    Tokeniser<'/'> uri_;
    void reset(StringView sv) noexcept
    {
        os_.reset();

        // Remove leading slash.
        if (sv.front() == '/')
            sv.remove_prefix(1);
        uri_.reset(sv);
    }

 public:
    void httpRequest(mg_connection& nc, mg::HttpMessage data)
    {
        reset(data.uri());

        if (!uri_.empty()) {

            auto tok = uri_.top();
            uri_.pop();

            if (tok == "rec") {
                recRequest(data);
            } else if (tok == "sess") {
                sessRequest(data);
            } else if (tok == "view") {
                viewRequest(data);
            } else {
                mg_serve_http(&nc, data.get(), httpOpts);
                return;
            }

            const auto len = static_cast<int>(os_.size());
            mg_printf(&nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%.*s", len, len,
                      os_.data());
        }
    }
    void recRequest(mg::HttpMessage data)
    {
        cout << "rec: " << data.method() << ' ' << data.uri() << endl;
        os_ << R"({"mnem":"EURUSD"})";
    }
    void sessRequest(mg::HttpMessage data)
    {
        cout << "sess: " << data.method() << ' ' << data.uri() << endl;
    }
    void viewRequest(mg::HttpMessage data)
    {
        cout << "view: " << data.method() << ' ' << data.uri() << endl;
    }
};

} // anonymous

int main(int argc, char* argv[])
{
    try {

        Mgr mgr;
        auto& conn = mgr.bind(HTTP_PORT);
        mg_set_protocol_http_websocket(&conn);

        httpOpts.document_root = ".";
        httpOpts.dav_document_root = ".";
        httpOpts.enable_directory_listing = "yes";

        cout << "Starting web server on port " << HTTP_PORT << endl;
        for (;;) {
            mgr.poll(1000);
        }

        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
