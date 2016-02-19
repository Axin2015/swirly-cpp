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
#include "Mock.hpp"
#include "RestServ.hpp"

#include <swirly/fir/Rest.hpp>

#include <swirly/ash/Log.hpp>
#include <swirly/ash/Time.hpp>

using namespace std;
using namespace swirly;

namespace {

constexpr char httpPort[] = "8080";
mg_serve_http_opts httpOpts;

volatile sig_atomic_t sig_{0};

void sigHandler(int sig) noexcept
{
  sig_ = sig;

  // Re-install signal handler.
  signal(sig, sigHandler);
}

} // anonymous

int main(int argc, char* argv[])
{
  int ret = 1;
  try {

    MockModel model;
    MockJourn journ;
    Rest rest{model, journ, getTimeOfDay()};

    mg::RestServ rs{rest, httpOpts};
    auto& conn = rs.bind(httpPort);
    mg_set_protocol_http_websocket(&conn);

    httpOpts.document_root = ".";
    httpOpts.dav_document_root = ".";
    httpOpts.enable_directory_listing = "yes";

    SWIRLY_NOTICE(logMsg() << "starting web server on port " << httpPort);

    signal(SIGHUP, sigHandler);
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    bool quit{false};
    do {
      rs.poll(1000);

      int sig{sig_};
      sig_ = 0;
      switch (sig) {
      case SIGHUP:
        SWIRLY_INFO("received SIGHUP"_sv);
        break;
      case SIGINT:
        SWIRLY_INFO("received SIGINT"_sv);
        quit = true;
        break;
      case SIGTERM:
        SWIRLY_INFO("received SIGTERM"_sv);
        quit = true;
        break;
      }
    } while (!quit);

    ret = 0;

  } catch (const exception& e) {
    SWIRLY_ERROR(logMsg() << "exception: " << e.what());
  }
  SWIRLY_NOTICE("exiting web server"_sv);
  return ret;
}
