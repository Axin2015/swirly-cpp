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
#include "Log.hpp"

#include <swirly/sys/Time.hpp>

#include <algorithm> // max()
#include <atomic>
#include <mutex>

#include <syslog.h>
#include <unistd.h> // getpid()

#include <sys/uio.h> // writev()

// The gettid() function is a Linux-specific function call.
#if defined(__linux__)
#include <sys/syscall.h>
inline pid_t gettid()
{
    return syscall(SYS_gettid);
}
#else
inline pid_t gettid()
{
    return getpid();
}
#endif

namespace swirly {
using namespace std;
namespace {

const char* labels_[] = {"CRIT", "ERROR", "WARNING", "NOTICE", "INFO", "DEBUG"};

// Global log level and logger function.
atomic<int> level_{LogInfo};
atomic<Logger> logger_{stdLogger};
mutex mutex_;

inline int acquireLevel() noexcept
{
    return level_.load(memory_order_acquire);
}

inline Logger acquireLogger() noexcept
{
    return logger_.load(memory_order_acquire);
}

} // namespace

const char* logLabel(int level) noexcept
{
    return labels_[min<int>(max<int>(level, LogCrit), LogDebug)];
}

int getLogLevel() noexcept
{
    return acquireLevel();
}

int setLogLevel(int level) noexcept
{
    return level_.exchange(max(level, 0), memory_order_acq_rel);
}

Logger getLogger() noexcept
{
    return acquireLogger();
}

Logger setLogger(Logger logger) noexcept
{
    return logger_.exchange(logger ? logger : nullLogger, memory_order_acq_rel);
}

void writeLog(int level, string_view msg) noexcept
{
    acquireLogger()(level, msg);
}

void nullLogger(int level, string_view msg) noexcept {}

void stdLogger(int level, string_view msg) noexcept
{
    const auto now = UnixClock::now();
    const auto t = UnixClock::to_time_t(now);
    const auto ms = msSinceEpoch(now);

    struct tm tm;
    localtime_r(&t, &tm);

    // The following format has an upper-bound of 42 characters:
    // "%b %d %H:%M:%S.%03d %-7s [%d]: "
    //
    // Example:
    // Mar 14 00:00:00.000 WARNING [0123456789]: msg...
    // <---------------------------------------->
    char head[42 + 1];
    size_t hlen = strftime(head, sizeof(head), "%b %d %H:%M:%S", &tm);
    hlen += sprintf(head + hlen, ".%03d %-7s [%d]: ", static_cast<int>(ms % 1000), logLabel(level),
                    static_cast<int>(gettid()));
    char tail{'\n'};
    iovec iov[] = {
        {head, hlen}, //
        {const_cast<char*>(msg.data()), msg.size()}, //
        {&tail, 1} //
    };

    int fd{level > LogWarning ? STDOUT_FILENO : STDERR_FILENO};
    // The following lock was required to avoid interleaving.
    lock_guard<mutex> lock{mutex_};
    // Best effort given that this is the logger.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    writev(fd, iov, sizeof(iov) / sizeof(iov[0]));
#pragma GCC diagnostic pop
}

void sysLogger(int level, string_view msg) noexcept
{
    int prio;
    switch (level) {
    case LogCrit:
        prio = LOG_CRIT;
        break;
    case LogError:
        prio = LOG_ERR;
        break;
    case LogWarning:
        prio = LOG_WARNING;
        break;
    case LogNotice:
        prio = LOG_NOTICE;
        break;
    case LogInfo:
        prio = LOG_INFO;
        break;
    default:
        prio = LOG_DEBUG;
    }
    syslog(prio, "%.*s", static_cast<int>(msg.size()), msg.data());
}

} // namespace swirly
