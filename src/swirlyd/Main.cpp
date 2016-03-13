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
#include <swirly/ash/System.hpp>
#include <swirly/ash/Time.hpp>

#include <boost/filesystem.hpp>

#include <iomanip>
#include <iostream>
#include <system_error>

#include <cerrno>

#include <fcntl.h> // open()
#include <syslog.h>
#include <unistd.h> // dup2()

#include <sys/stat.h> // umask()

using namespace std;
using namespace swirly;

namespace fs = boost::filesystem;

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
  if (fd < 0) {
    throw system_error(errno, system_category(), "open() failed");
  }

  dup2(fd, STDOUT_FILENO);
  dup2(fd, STDERR_FILENO);
  close(fd);
}

mode_t getMask()
{
  mode_t mask{umask(0)};
  umask(mask);
  return mask;
}

struct Opts {
  const char* authUser{"Auth-User"};
  fs::path directory;
  const char* httpPort{"8080"};
  fs::path logFile;
  const char* mask{nullptr};
  bool daemon{true};
};

void printUsage(ostream& os)
{
  os << R"==(Usage: swirlyd [options]

Options:
  -h                Show this help message.
  -e level          Set log-level (0-5). Default is 4.
  -d directory      Set working directory. Default is '/' unless -n.
  -l path           Write to log-file. Specify '-' for default path.
  -n                Do not daemonise. I.e. run in the foreground.
  -m mode           File creation mode mask. Default is 0027 unless -n.
  -u name           Http auth-user header. Default is 'Auth-User'.
  -p port           Http port. Default is 8080.

Report bugs to: support@swirlycloud.com
)==";
}

void getOpts(int argc, char* argv[], Opts& opts)
{
  opterr = 0;
  int ch;
  while ((ch = getopt(argc, argv, ":d:e:hl:m:np:u:")) != -1) {
    switch (ch) {
    case 'd':
      opts.directory = optarg;
      break;
    case 'e':
      setLogLevel(atoi(optarg));
      break;
    case 'h':
      printUsage(cout);
      exit(0);
    case 'l':
      if (strcmp(optarg, "-") == 0) {
        // Use default log-file path.
        opts.logFile = "/var/log/swirlyd/swirlyd.log";
      } else {
        opts.logFile = optarg;
      }
      break;
    case 'm':
      opts.mask = optarg;
      break;
    case 'n':
      opts.daemon = false;
      break;
    case 'p':
      opts.httpPort = optarg;
      break;
    case 'u':
      opts.authUser = optarg;
      break;
    case ':':
      cerr << "Option '" << static_cast<char>(optopt) << "' requires an argument\n";
      printUsage(cerr);
      exit(1);
    case '?':
    default:
      cerr << "Unknown option '" << static_cast<char>(optopt) << "'\n";
      printUsage(cerr);
      exit(1);
    }
  }

  if (optind < argc) {
    cerr << "Unknown argument '" << argv[optind] << "'\n";
    printUsage(cerr);
    exit(1);
  }
}

} // anonymous

int main(int argc, char* argv[])
{
  int ret = 1;
  try {

    Opts opts;
    getOpts(argc, argv, opts);

    // Restrict file creation mask if specified. This function is always successful.
    if (opts.mask) {
      // Zero base to auto-detect: if the prefix is 0, the base is octal, if the prefix is 0x or 0X.
      umask(static_cast<mode_t>(strtol(opts.mask, nullptr, 0)));
    } else if (opts.daemon) {
      umask(0027);
    }

    if (!opts.directory.empty()) {
      // Change the current working directory if specified.
      opts.directory = fs::canonical(opts.directory, fs::current_path());
      if (chdir(opts.directory.c_str()) < 0) {
        throw system_error(errno, system_category(), "chdir() failed");
      }
    } else if (opts.daemon) {
      // Default to root directory if daemon.
      opts.directory = fs::current_path().root_path();
    } else {
      // Otherwise, default to current directory.
      opts.directory = fs::current_path();
    }

    if (opts.daemon) {

      // Daemonise process.
      daemon();

      // Daemon uses syslog by default.
      if (opts.logFile.empty()) {
        openlog("swirlyd", LOG_PID | LOG_NDELAY, LOG_LOCAL0);
        setlogmask(LOG_UPTO(LOG_DEBUG));
        setLogger(sysLogger);
      }
    }

    if (!opts.logFile.empty()) {

      // Log file is relative to working directory. We use absolute, rather than canonical here,
      // because canonical requires that the file exists.
      if (opts.logFile.is_relative()) {
        opts.logFile = fs::absolute(opts.logFile, opts.directory);
      }

      fs::create_directory(opts.logFile.parent_path());

      SWIRLY_NOTICE(logMsg() << "opening log file: " << opts.logFile);
      openLogFile(opts.logFile.c_str());
    }

    MockModel model;
    MockJourn journ;
    Rest rest{model, journ, getTimeOfDay()};

    mg::RestServ rs{rest, httpOpts};
    auto& conn = rs.bind(opts.httpPort);
    mg_set_protocol_http_websocket(&conn);

    httpOpts.document_root = ".";
    httpOpts.dav_document_root = ".";
    httpOpts.enable_directory_listing = "yes";

    SWIRLY_NOTICE(logMsg() << "started swirlyd server on port " << opts.httpPort);

    SWIRLY_INFO(logMsg() << "auth-user: " << opts.authUser);
    SWIRLY_INFO(logMsg() << "directory: " << opts.directory);
    SWIRLY_INFO(logMsg() << "http-port: " << opts.httpPort);
    SWIRLY_INFO(logMsg() << "log-file:  " << opts.logFile);
    SWIRLY_INFO(logMsg() << "log-level: " << getLogLevel());
    SWIRLY_INFO(logMsg() << "daemon:    " << (opts.daemon ? "yes" : "no"));
    SWIRLY_INFO(logMsg() << "umask:     0" << setfill('0') << setw(3) << oct << getMask());

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
        if (!opts.logFile.empty()) {
          SWIRLY_NOTICE(logMsg() << "reopening log file: " << opts.logFile);
          openLogFile(opts.logFile.c_str());
        }
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
  SWIRLY_NOTICE("stopped swirlyd server"_sv);
  return ret;
}
