/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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

#include <swirly/sqlite/Journ.hpp>
#include <swirly/sqlite/Model.hpp>

#include <swirly/web/Rest.hpp>

#include <swirly/fin/Journ.hpp>
#include <swirly/fin/Model.hpp>
#include <swirly/fin/MsgQueue.hpp>

#include <swirly/app/MemCtx.hpp>
#include <swirly/app/Thread.hpp>

#include <swirly/sys/Daemon.hpp>
#include <swirly/sys/EpollReactor.hpp>
#include <swirly/sys/File.hpp>
#include <swirly/sys/PidFile.hpp>
#include <swirly/sys/Signal.hpp>
#include <swirly/sys/System.hpp>

#include <swirly/util/Config.hpp>
#include <swirly/util/Exception.hpp>
#include <swirly/util/Finally.hpp>
#include <swirly/util/Log.hpp>

#include <experimental/filesystem>

#include <fstream>
#include <iomanip>
#include <iostream>

#include <fcntl.h> // open()
#include <syslog.h>
#include <unistd.h> // dup2()

namespace fs = std::experimental::filesystem;

using namespace std;
using namespace swirly;

namespace {

void openLogFile(const char* path)
{
    const auto file = os::open(path, O_RDWR | O_CREAT | O_APPEND, 0644);
    dup2(*file, STDOUT_FILENO);
    dup2(*file, STDERR_FILENO);
}

struct Opts {
    fs::path confFile;
    bool daemon{false};
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
            opts.startTime = toTime(Millis{stou64(optarg)});
            break;
        case ':':
            cerr << "Option '"sv << static_cast<char>(optopt) << "' requires an argument\n"sv;
            printUsage(cerr);
            exit(1);
        case '?':
        default:
            cerr << "Unknown option '"sv << static_cast<char>(optopt) << "'\n"sv;
            printUsage(cerr);
            exit(1);
        }
    }

    if (optind < argc) {
        cerr << "Unknown argument '"sv << argv[optind] << "'\n"sv;
        printUsage(cerr);
        exit(1);
    }
}

MemCtx memCtx;

} // namespace

namespace swirly {

void* alloc(size_t size)
{
    return memCtx.alloc(size);
}

void* alloc(size_t size, align_val_t al)
{
    return memCtx.alloc(size, al);
}

void dealloc(void* ptr, size_t size) noexcept
{
    return memCtx.dealloc(ptr, size);
}

} // namespace swirly

int main(int argc, char* argv[])
{
    closeAll();

    int ret = 1;
    try {

        Opts opts;
        getOpts(argc, argv, opts);

        if (opts.startTime == Time{}) {
            opts.startTime = UnixClock::now();
        }

        Config config;
        if (!opts.confFile.empty()) {
            ifstream is{opts.confFile};
            if (!is.is_open()) {
                throw Exception{errMsg() << "open failed: "sv << opts.confFile};
            }
            config.read(is);
        }

        memCtx = MemCtx{config.get<size_t>("mem_size", 1) << 20};

        const auto logLevel = config.get("log_level", ""sv);
        if (!logLevel.empty()) {
            setLogLevel(fromString<int>(logLevel));
        }

        // Restrict file creation mask if specified. The umask function is always successful.
        const auto fileMode = config.get("file_mode", ""sv);
        if (!fileMode.empty()) {
            // Zero base to auto-detect: if the prefix is 0, the base is octal, if the prefix is 0x or 0X.
            umask(fromString<mode_t>(fileMode));
        } else if (opts.daemon) {
            umask(0027);
        }

        fs::path runDir{config.get("run_dir", "")};
        if (!runDir.empty()) {
            // Change the current working directory if specified.
            runDir = fs::canonical(runDir, fs::current_path());
            os::chdir(runDir.c_str());
        } else if (opts.daemon) {
            // Default to root directory if daemon.
            runDir = fs::current_path().root_path();
        } else {
            // Otherwise, default to current directory.
            runDir = fs::current_path();
        }

        fs::path logFile{config.get("log_file", "")};
        fs::path pidFile{config.get("pid_file", "")};

        PidFile pf;
        if (!pidFile.empty()) {

            // Pid file is relative to working directory. We use absolute, rather than canonical
            // here, because canonical requires that the file exists.
            if (pidFile.is_relative()) {
                pidFile = fs::absolute(pidFile, runDir);
            }
            fs::create_directory(pidFile.parent_path());
            pf = openPidFile(pidFile.c_str(), 0644);
        }

        if (opts.daemon) {
            // Daemonise process.
            daemon();
        }

        writePidFile(pf);
        if (logFile.empty()) {

            // If a log file is not specified, then use syslog().

#if SWIRLY_BUILD_DEBUG
            setlogmask(LOG_UPTO(LOG_DEBUG));
#endif
            setLogger(sysLogger);

        } else if (logFile == "-") {

            // A single dash in place of a file name is used to indicate stdout/stderr.
            // This is effectively a no-op and there is no need to open a file.

            // Clear so that SIGHUP does not attempt to reopen the log file.
            logFile.clear();

        } else {

            // Log file is relative to working directory. We use absolute, rather than canonical
            // here, because canonical requires that the file exists.
            if (logFile.is_relative()) {
                logFile = fs::absolute(logFile, runDir);
            }
            fs::create_directory(logFile.parent_path());

            SWIRLY_NOTICE << "opening log file: "sv << logFile;
            openLogFile(logFile.c_str());
        }

        const fs::path mqFile{config.get("mq_file", "")};
        const char* const httpPort{config.get("http_port", "8080")};
        const auto maxExecs = config.get<size_t>("max_execs", 1 << 4);

        SWIRLY_NOTICE << "initialising daemon"sv;
        SWIRLY_INFO << "conf_file:     "sv << opts.confFile;
        SWIRLY_INFO << "daemon:        "sv << (opts.daemon ? "yes"sv : "no"sv);
        SWIRLY_INFO << "start_time:    "sv << opts.startTime;

        SWIRLY_INFO << "file_mode:     "sv << setfill('0') << setw(3) << oct << swirly::fileMode();
        SWIRLY_INFO << "http_port:     "sv << httpPort;
        SWIRLY_INFO << "log_file:      "sv << logFile;
        SWIRLY_INFO << "log_level:     "sv << getLogLevel();
        SWIRLY_INFO << "max_execs:     "sv << maxExecs;
        SWIRLY_INFO << "mem_size:      "sv << (memCtx.maxSize() >> 20) << "MiB"sv;
        SWIRLY_INFO << "mq_file:       "sv << mqFile;
        SWIRLY_INFO << "pid_file:      "sv << pidFile;
        SWIRLY_INFO << "run_dir:       "sv << runDir;

        MsgQueue mq;
        if (!mqFile.empty()) {
            mq = MsgQueue{mqFile.c_str()};
        } else {
            mq = MsgQueue{1 << 10};
        }
        Rest rest{mq, maxExecs};
        {
            SqlModel model{config};
            rest.load(model, opts.startTime);
        }
        RestServ restServ{rest};
        SqlJourn journ{config};

        EpollReactor reactor{1024};
        const TcpEndpoint ep{Tcp::v4(), stou16(httpPort)};
        HttpServ httpServ{reactor, ep, restServ};

        ReactorThread reactorThread{reactor, ThreadConfig{"reactor"s}};
        auto journAgent = [&mq, &journ]() {
            int n{0};
            Msg msg;
            while (mq.pop(msg)) {
                journ.write(msg);
                ++n;
            }
            return n;
        };
        AgentThread journThread{journAgent, ThreadConfig{"journ"s}};

        SWIRLY_NOTICE << "started http server on port "sv << httpPort;

        // Wait for termination.
        SigWait sigWait;
        while (const auto sig = sigWait()) {
            switch (sig) {
            case SIGHUP:
                SWIRLY_INFO << "received SIGHUP"sv;
                if (!logFile.empty()) {
                    SWIRLY_NOTICE << "reopening log file: "sv << logFile;
                    openLogFile(logFile.c_str());
                }
                continue;
            case SIGINT:
                SWIRLY_INFO << "received SIGINT"sv;
                break;
            case SIGTERM:
                SWIRLY_INFO << "received SIGTERM"sv;
                break;
            default:
                SWIRLY_INFO << "received signal: "sv << sig;
                continue;
            }
            break;
        }
        ret = 0;
    } catch (const exception& e) {
        SWIRLY_ERROR << "exception: "sv << e.what();
    }
    return ret;
}
