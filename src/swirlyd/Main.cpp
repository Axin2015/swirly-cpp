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
// Older versions of program_options reference auto_ptr even when BOOST_NO_AUTO_PTR is set.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <boost/program_options.hpp>
#pragma GCC diagnostic pop

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

mode_t getUMask()
{
  mode_t mask{umask(0)};
  umask(mask);
  return mask;
}

} // anonymous

int main(int argc, char* argv[])
{
  namespace fs = boost::filesystem;
  namespace po = boost::program_options;

  int ret = 1;
  try {

    string authUser;
    fs::path directory;
    string httpPort;
    fs::path logFile;
    int logLevel;
    string umask;

    po::options_description generalDesc{"General options"};
    generalDesc.add_options() //
      ("help,h", //
       "show help message") //
      ("log-level,e", po::value<int>(&logLevel)->default_value(LogInfo), //
       "log level (0-5)") //
      ;

    po::options_description daemonDesc{"Daemon options"};
    daemonDesc.add_options() //
      ("directory,d", po::value<fs::path>(&directory), //
       "working directory") //
      ("log-file,l", po::value<fs::path>(&logFile) //
                       ->implicit_value("/var/log/swirlyd/swirlyd.log"), //
       "log file name") //
      ("no-daemon,n", //
       "run in the foreground") //
      ("umask,m", po::value<string>(&umask), //
       "file creation mask") //
      ;

    po::options_description httpDesc{"Http options"};
    httpDesc.add_options() //
      ("auth-user,u", po::value<string>(&authUser)->default_value("Auth-User"), //
       "http auth-user header") //
      ("http-port,p", po::value<string>(&httpPort)->default_value("8080"), //
       "http port") //
      ;

    po::options_description desc{"Allowed options"};
    desc.add(generalDesc).add(daemonDesc).add(httpDesc);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << "Usage: swirlyd [options]\n" << desc << endl;
      return 1;
    }

    const bool noDaemon{vm.count("no-daemon") > 0};

    setLogLevel(logLevel);

    // Restrict file creation mask if specified. This function is always successful.
    if (!umask.empty()) {
      // Zero base to auto-detect: if the prefix is 0, the base is octal, if the prefix is 0x or 0X.
      ::umask(static_cast<mode_t>(stoi(umask, nullptr, 0)));
    } else if (!noDaemon) {
      ::umask(0027);
    }

    if (!directory.empty()) {
      // Change the current working directory if specified.
      directory = fs::canonical(directory, fs::current_path());
      if (chdir(directory.c_str()) < 0) {
        throw system_error(errno, system_category(), "chdir() failed");
      }
    } else if (!noDaemon) {
      // Default to root directory if daemon.
      directory = fs::current_path().root_path();
    } else {
      // Otherwise, default to current directory.
      directory = fs::current_path();
    }

    if (!noDaemon) {

      // Daemonise process.
      daemon();

      // Daemon uses syslog by default.
      if (logFile.empty()) {
        openlog("swirlyd", LOG_PID | LOG_NDELAY, LOG_LOCAL0);
        setlogmask(LOG_UPTO(LOG_DEBUG));
        setLogger(sysLogger);
      }
    }

    if (!logFile.empty()) {

      // Log file is relative to working directory. We use absolute, rather than canonical here,
      // because canonical requires that the file exists.
      if (logFile.is_relative()) {
        logFile = fs::absolute(logFile, directory);
      }

      fs::create_directory(logFile.parent_path());

      SWIRLY_NOTICE(logMsg() << "opening log file: " << logFile);
      openLogFile(logFile.c_str());
    }

    MockModel model;
    MockJourn journ;
    Rest rest{model, journ, getTimeOfDay()};

    mg::RestServ rs{rest, httpOpts};
    auto& conn = rs.bind(httpPort.c_str());
    mg_set_protocol_http_websocket(&conn);

    httpOpts.document_root = ".";
    httpOpts.dav_document_root = ".";
    httpOpts.enable_directory_listing = "yes";

    SWIRLY_NOTICE(logMsg() << "started swirlyd server on port " << httpPort);

    SWIRLY_INFO(logMsg() << "auth-user: " << authUser);
    SWIRLY_INFO(logMsg() << "directory: " << directory);
    SWIRLY_INFO(logMsg() << "http-port: " << httpPort);
    SWIRLY_INFO(logMsg() << "log-file:  " << logFile);
    SWIRLY_INFO(logMsg() << "log-level: " << logLevel);
    SWIRLY_INFO(logMsg() << "no-daemon: " << noDaemon);
    SWIRLY_INFO(logMsg() << "umask:     0" << setfill('0') << setw(3) << oct << getUMask());

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
        if (!logFile.empty()) {
          SWIRLY_NOTICE(logMsg() << "reopening log file: " << logFile);
          openLogFile(logFile.c_str());
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
