/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#include "HttpServ.hpp"
#include "RestServ.hpp"

#include <swirly/clob/Test.hpp>

#include <swirly/ws/Rest.hpp>

#include <swirly/fin/Journ.hpp>
#include <swirly/fin/Model.hpp>

#include <swirly/util/Conf.hpp>
#include <swirly/util/Exception.hpp>
#include <swirly/util/Log.hpp>
#include <swirly/util/MemCtx.hpp>
#include <swirly/util/System.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#pragma GCC diagnostic pop

#include <iomanip>
#include <iostream>

#include <fcntl.h> // open()
#include <syslog.h>
#include <unistd.h> // dup2()

using namespace std;
using namespace swirly;

namespace fs = boost::filesystem;

namespace {

void openLogFile(const char* path)
{
    const int fd{open(path, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP)};
    if (fd < 0) {
        throw system_error{errno, system_category(), "open failed"};
    }

    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);
}

class SigHandler {
  public:
    SigHandler(boost::asio::io_service& ioServ, const fs::path& logFile)
        : ioServ_(ioServ), signals_{ioServ}, logFile_{logFile}
    {
        signals_.add(SIGHUP);
        signals_.add(SIGINT);
        signals_.add(SIGTERM);

        wait();
    }
    ~SigHandler() noexcept = default;

    // Copy.
    SigHandler(const SigHandler&) = delete;
    SigHandler& operator=(const SigHandler&) = delete;

    // Move.
    SigHandler(SigHandler&&) = delete;
    SigHandler& operator=(SigHandler&&) = delete;

  private:
    void wait() noexcept
    {
        // FIXME: implement clean shutdown that attempts to deliver pending messages on a best effort
        // basis.
        signals_.async_wait([this](auto ec, int sig) {
            if (!ec) {
                switch (sig) {
                case SIGHUP:
                    SWIRLY_INFO("received SIGHUP"_sv);
                    if (!logFile_.empty()) {
                        SWIRLY_NOTICE(logMsg() << "reopening log file: " << logFile_);
                        openLogFile(logFile_.c_str());
                    }
                    break;
                case SIGINT:
                    SWIRLY_INFO("received SIGINT"_sv);
                    ioServ_.stop();
                    break;
                case SIGTERM:
                    SWIRLY_INFO("received SIGTERM"_sv);
                    ioServ_.stop();
                    break;
                }
            }
            this->wait();
        });
    }

    boost::asio::io_service& ioServ_;
    boost::asio::signal_set signals_;
    fs::path logFile_;
};

struct Opts {
    fs::path confFile;
    bool daemon{false};
    bool test{false};
    Time startTime{};
};

void printUsage(ostream& os)
{
    os << R"==(Usage: swirlyd [options]

Options:
  -h           Show this help message.
  -d           Run in the background as a daemon process.
  -f path      Path to configuration file.
  -s time      Initial time of day in millis since epoch.
  -t           Test mode.

Report bugs to: support@swirlycloud.com
)==";
}

void getOpts(int argc, char* argv[], Opts& opts)
{
    opterr = 0;
    int ch;
    while ((ch = getopt(argc, argv, ":df:hs:t")) != -1) {
        switch (ch) {
        case 'd':
            opts.daemon = true;
            break;
        case 'f':
            opts.confFile = optarg;
            break;
        case 'h':
            printUsage(cout);
            exit(0);
        case 's':
            opts.startTime = msToTime(stou64(optarg));
            break;
        case 't':
            opts.test = true;
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

MemCtx memCtx;

} // anonymous

namespace swirly {

void* alloc(size_t size)
{
    return memCtx.alloc(size);
}

void dealloc(void* ptr, size_t size) noexcept
{
    return memCtx.dealloc(ptr, size);
}

} // swirly

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        Opts opts;
        getOpts(argc, argv, opts);

        if (opts.startTime == Time{}) {
            opts.startTime = UnixClock::now();
        }

        Conf conf;
        if (!opts.confFile.empty()) {
            fs::ifstream is{opts.confFile};
            if (!is.is_open()) {
                throw Exception{errMsg() << "open failed: " << opts.confFile};
            }
            conf.read(is);
        }

        memCtx = MemCtx{conf.get<size_t>("mem_size", 1) << 20};

        const char* const logLevel{conf.get("log_level", nullptr)};
        if (logLevel) {
            setLogLevel(atoi(logLevel));
        }

        // Restrict file creation mask if specified. The umask function is always successful.
        const char* const fileMode{conf.get("file_mode", nullptr)};
        if (fileMode) {
            // Zero base to auto-detect: if the prefix is 0, the base is octal, if the prefix is 0x or 0X.
            umask(numericCast<mode_t>(fileMode));
        } else if (opts.daemon) {
            umask(0027);
        }

        fs::path runDir{conf.get("run_dir", "")};
        if (!runDir.empty()) {
            // Change the current working directory if specified.
            runDir = fs::canonical(runDir, fs::current_path());
            if (chdir(runDir.c_str()) < 0) {
                throw system_error{errno, system_category(), "chdir failed"};
            }
        } else if (opts.daemon) {
            // Default to root directory if daemon.
            runDir = fs::current_path().root_path();
        } else {
            // Otherwise, default to current directory.
            runDir = fs::current_path();
        }

        fs::path logFile{conf.get("log_file", "")};
        if (opts.daemon) {

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
                logFile = fs::absolute(logFile, runDir);
            }

            fs::create_directory(logFile.parent_path());

            SWIRLY_NOTICE(logMsg() << "opening log file: " << logFile);
            openLogFile(logFile.c_str());
        }

        const char* const httpPort{conf.get("http_port", "8080")};
        const auto pipeCapacity = conf.get<size_t>("pipe_capacity", 1 << 10);
        const auto maxExecs = conf.get<size_t>("max_execs", 1 << 4);

        SWIRLY_NOTICE("initialising daemon");
        SWIRLY_INFO(logMsg() << "conf_file:     " << opts.confFile);
        SWIRLY_INFO(logMsg() << "daemon:        " << (opts.daemon ? "yes" : "no"));
        SWIRLY_INFO(logMsg() << "start_time:    " << opts.startTime);
        SWIRLY_INFO(logMsg() << "test_mode:     " << (opts.test ? "yes" : "no"));

        SWIRLY_INFO(logMsg() << "mem_size:      " << (memCtx.maxSize() >> 20) << "MiB");
        SWIRLY_INFO(logMsg() << "file_mode:     " << setfill('0') << setw(3) << oct
                             << swirly::fileMode());
        SWIRLY_INFO(logMsg() << "run_dir:       " << runDir);
        SWIRLY_INFO(logMsg() << "log_file:      " << logFile);
        SWIRLY_INFO(logMsg() << "log_level:     " << getLogLevel());
        SWIRLY_INFO(logMsg() << "http_port:     " << httpPort);
        SWIRLY_INFO(logMsg() << "pipe_capacity: " << pipeCapacity);
        SWIRLY_INFO(logMsg() << "max_execs:     " << maxExecs);

        unique_ptr<Journ> journ;
        if (!opts.test) {
            journ = swirly::makeJourn(conf);
        } else {
            journ = make_unique<TestJourn>();
        }
        auto model = swirly::makeModel(conf);
        Rest rest{*journ, pipeCapacity, maxExecs};
        rest.load(*model, opts.startTime);
        model = nullptr;

        boost::asio::io_service ioServ;
        SigHandler sigHandler{ioServ, logFile};

        RestServ restServ{rest};
        HttpServ serv{ioServ, stou16(httpPort), restServ};

        SWIRLY_NOTICE(logMsg() << "started http server on port " << httpPort);
        ioServ.run();
        ret = 0;

    } catch (const exception& e) {
        SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
    SWIRLY_NOTICE("stopped http server"_sv);
    return ret;
}
