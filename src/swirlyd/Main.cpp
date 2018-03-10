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

#include <swirly/clob/Test.hpp>

#include <swirly/web/Rest.hpp>

#include <swirly/fin/Journ.hpp>
#include <swirly/fin/Model.hpp>

#include <swirly/util/Config.hpp>
#include <swirly/util/Exception.hpp>
#include <swirly/util/File.hpp>
#include <swirly/util/Finally.hpp>
#include <swirly/util/Log.hpp>

#include <swirly/sys/Daemon.hpp>
#include <swirly/sys/File.hpp>
#include <swirly/sys/MemCtx.hpp>
#include <swirly/sys/PidFile.hpp>
#include <swirly/sys/Signal.hpp>
#include <swirly/sys/System.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#pragma GCC diagnostic pop

#include <csignal>
#include <iomanip>
#include <iostream>
#include <thread>

#include <fcntl.h> // open()
#include <syslog.h>
#include <unistd.h> // dup2()

using namespace std;
using namespace swirly;

namespace fs = boost::filesystem;

namespace {

void openLogFile(const char* path)
{
    const auto file = sys::open(path, O_RDWR | O_CREAT | O_APPEND, 0644);
    dup2(*file, STDOUT_FILENO);
    dup2(*file, STDERR_FILENO);
}

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
            opts.startTime = toTime(Millis{stou64(optarg)});
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
volatile sig_atomic_t quit{0};

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
    signal(SIGINT, [](int) { ++quit; });

    int ret = 1;
    try {

        Opts opts;
        getOpts(argc, argv, opts);

        if (opts.startTime == Time{}) {
            opts.startTime = UnixClock::now();
        }

        Config config;
        if (!opts.confFile.empty()) {
            fs::ifstream is{opts.confFile};
            if (!is.is_open()) {
                throw Exception{errMsg() << "open failed: " << opts.confFile};
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
            sys::chdir(runDir.c_str());
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

#if SWIRLY_ENABLE_DEBUG
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

            SWIRLY_NOTICE(logMsg() << "opening log file: " << logFile);
            openLogFile(logFile.c_str());
        }

        const char* const httpPort{config.get("http_port", "8080")};
        const auto pipeCapacity = config.get<size_t>("pipe_capacity", 1 << 10);
        const auto maxExecs = config.get<size_t>("max_execs", 1 << 4);

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
            journ = make_unique<SqlJourn>(config);
        } else {
            journ = make_unique<TestJourn>();
        }
        Rest rest{*journ, pipeCapacity, maxExecs};
        {
            SqlModel model{config};
            rest.load(model, opts.startTime);
        }

        RestServ restServ{rest};
        EpollReactor reactor{1024};

        const TcpEndpoint ep{Tcp::v4(), stou16(httpPort)};
        HttpServ::make(reactor, ep, restServ);

        const auto fn = [](Reactor& r) {
            sigBlockAll();
            SWIRLY_NOTICE("started reactor thread"sv);
            try {
                while (!r.closed()) {
                    r.poll();
                }
            } catch (const exception& e) {
                SWIRLY_ERROR(logMsg() << "exception: " << e.what());
            }
            SWIRLY_NOTICE("stopped reactor thread"sv);
        };
        auto worker = thread{fn, ref(reactor)};
        auto finally = makeFinally([&]() {
            reactor.close();
            worker.join();
            SWIRLY_NOTICE("stopped http server"sv);
        });
        SWIRLY_NOTICE(logMsg() << "started http server on port " << httpPort);

        // Wait for termination.
        SigWait sigWait;
        while (const auto sig = sigWait()) {
            switch (sig) {
            case SIGHUP:
                SWIRLY_INFO("received SIGHUP"sv);
                if (!logFile.empty()) {
                    SWIRLY_NOTICE(logMsg() << "reopening log file: " << logFile);
                    openLogFile(logFile.c_str());
                }
                continue;
            case SIGINT:
                SWIRLY_INFO("received SIGINT"sv);
                break;
            case SIGTERM:
                SWIRLY_INFO("received SIGTERM"sv);
                break;
            default:
                SWIRLY_INFO(logMsg() << "received signal: " << sig);
                continue;
            }
            break;
        }
        ret = 0;

    } catch (const exception& e) {
        SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
    return ret;
}
