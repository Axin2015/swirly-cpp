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
#include <swirly/ash/String.hpp>

#include <iostream>

using namespace std;

namespace mg {

swirly::StringView toView(const mg_str& str) noexcept
{
    return {str.p, str.len};
}

class Error : public swirly::Exception {
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
        return toView(impl_->method);
    }
    auto uri() const noexcept
    {
        return toView(impl_->uri);
    }
    auto proto() const noexcept
    {
        return toView(impl_->proto);
    }
    auto queryString() const noexcept
    {
        return toView(impl_->query_string);
    }
    auto body() const noexcept
    {
        return toView(impl_->body);
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
            swirly::throwException<Error>("mg_bind() failed");
        conn->user_data = this;
        return *conn;
    }
    time_t poll(int milli)
    {
        return mg_mgr_poll(&mgr_, milli);
    }
};

} // mg

namespace {

constexpr char HTTP_PORT[] = "8000";

static struct mg_serve_http_opts httpOpts;

class Mgr : public mg::MgrBase<Mgr> {
 public:
    void httpRequest(mg_connection& nc, mg::HttpMessage data)
    {
        cout << data.method() << ' ' << data.uri() << endl;
        mg_serve_http(&nc, data.get(), httpOpts);
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
