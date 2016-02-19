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

#include <swirly/ash/Daemon.hpp>
#include <swirly/ash/Log.hpp>
#include <swirly/ash/Time.hpp>

#include <boost/program_options.hpp>

#include <iostream>
#include <system_error>

#include <cerrno>

#include <fcntl.h> // open()
#include <syslog.h>
#include <unistd.h> // dup2()

using namespace std;
using namespace swirly;

namespace {

mg_serve_http_opts httpOpts;

volatile sig_atomic_t sig_{0};

void sigHandler(int sig) noexcept
{
  sig_ = sig;

  // Re-install signal handler.
  signal(sig, sigHandler);
}

void openLogFile(const char* path)
{
  const int fd{open(path, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP)};
  if (fd < 0)
    throw system_error(errno, system_category(), "open() failed");

  dup2(fd, STDOUT_FILENO);
  dup2(fd, STDERR_FILENO);
  close(fd);
}

} // anonymous

int main(int argc, char* argv[])
{
  namespace po = boost::program_options;

  int ret = 1;
  try {

    string httpPort;
    string logFile;
    string workDir;

    po::options_description generalDesc{"General options"};
    generalDesc.add_options() //
      ("help,h", //
       "show help message") //
      ;

    po::options_description daemonDesc{"Daemon options"};
    daemonDesc.add_options() //
      ("daemon,d", //
       "daemonise process") //
      ("logfile,l", po::value<string>(&logFile)->implicit_value("swirly_httpd.log"), //
       "log file name") //
      ("working,w", po::value<string>(&workDir)->default_value("/"), //
       "working directory") //
      ;

    po::options_description httpDesc{"Http options"};
    httpDesc.add_options() //
      ("port,p", po::value<string>(&httpPort)->default_value("8080"), //
       "http port") //
      ;

    po::options_description desc{"Allowed options"};
    desc.add(generalDesc).add(daemonDesc).add(httpDesc);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << "Usage: swirly_httpd [options]\n" << desc << endl;
      return 1;
    }

    if (vm.count("daemon")) {
      daemon(workDir.c_str(), 0027);
      if (logFile.empty()) {
        // Daemon uses syslog by default.
        openlog("swirly_httpd", LOG_PID | LOG_NDELAY, LOG_LOCAL0);
        setLogger(sysLogger);
      }
    }

    if (!logFile.empty())
      openLogFile(logFile.c_str());

    MockModel model;
    MockJourn journ;
    Rest rest{model, journ, getTimeOfDay()};

    mg::RestServ rs{rest, httpOpts};
    auto& conn = rs.bind(httpPort.c_str());
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
        if (!logFile.empty())
          openLogFile(logFile.c_str());
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
