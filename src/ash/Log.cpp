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
#include <swirly/ash/Log.hpp>

#include <swirly/ash/Time.hpp>
#include <swirly/ash/Types.hpp>

#include <algorithm> // max()
#include <atomic>
#include <cstring> // strlen()

#include <syslog.h>
#include <unistd.h> // getpid()

#include <sys/uio.h> // writev()

using namespace std;

namespace swirly {
namespace {

const char* labels_[] = {"CRIT", "ERROR", "WARN", "NOTICE", "INFO", "DEBUG"};

// Global log level and logger function.
atomic<int> level_{LogInfo};
atomic<Logger> logger_{stdLogger};

thread_local LogMsg logMsg_;

inline int acquireLevel() noexcept
{
  return level_.load(memory_order_acquire);
}

inline Logger acquireLogger() noexcept
{
  return logger_.load(memory_order_acquire);
}

} // anonymous

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

void writeLog(int level, const std::string_view& msg) noexcept
{
  acquireLogger()(level, msg);
}

void nullLogger(int level, const std::string_view& msg) noexcept
{
}

void stdLogger(int level, const std::string_view& msg) noexcept
{
  const Millis ms{getTimeOfDay()};
  const time_t now{unbox(ms) / 1000};

  struct tm tm;
  localtime_r(&now, &tm);

  // The following format has an upper-bound of 41 characters:
  // "%b %d %H:%M:%S.%03d %-6s [%d]: "
  char head[42];
  size_t hlen = strftime(head, sizeof(head), "%b %d %H:%M:%S", &tm);
  hlen += sprintf(head + hlen, ".%03d %-6s [%d]: ", static_cast<int>(unbox(ms) % 1000),
                  logLabel(level), static_cast<int>(getpid()));
  char tail = '\n';
  iovec iov[] = {
    {head, hlen}, //
    {const_cast<char*>(msg.data()), msg.size()}, //
    {&tail, 1} //
  };

  int fd{level > LogWarn ? STDOUT_FILENO : STDERR_FILENO};
  // Best effort given that this is the logger.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
  writev(fd, iov, sizeof(iov) / sizeof(iov[0]));
#pragma GCC diagnostic pop
}

void sysLogger(int level, const std::string_view& msg) noexcept
{
  int prio;
  switch (level) {
  case LogCrit:
    prio = LOG_CRIT;
    break;
  case LogError:
    prio = LOG_ERR;
    break;
  case LogWarn:
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

LogMsg& logMsg() noexcept
{
  logMsg_.reset();
  return logMsg_;
}

} // swirly
