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

#include <iostream>

using namespace std;

namespace {

constexpr char HTTP_PORT[] = "8000";

static struct mg_serve_http_opts httpOpts;

void ev_handler(mg_connection* nc, int ev, void* p)
{
    if (ev == MG_EV_HTTP_REQUEST) {
        mg_serve_http(nc, static_cast<http_message*>(p), httpOpts);
    }
}

} // anonymous

int main(int argc, char* argv[])
{
    try {
        mg_mgr mgr;
        mg_connection* nc;

        mg_mgr_init(&mgr, nullptr);
        nc = mg_bind(&mgr, HTTP_PORT, ev_handler);

        mg_set_protocol_http_websocket(nc);
        httpOpts.document_root = ".";
        httpOpts.dav_document_root = ".";
        httpOpts.enable_directory_listing = "yes";

        cout << "Starting web server on port " << HTTP_PORT << endl;
        for (;;) {
            mg_mgr_poll(&mgr, 1000);
        }
        mg_mgr_free(&mgr);

        return 0;
    } catch (const exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
