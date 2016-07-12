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
#ifndef SWIRLY_ASH_LOG_HPP
#define SWIRLY_ASH_LOG_HPP

#include <swirly/ash/Stream.hpp>

namespace swirly {

/**
 * Maximum log message length when LogMsg is used.
 */
constexpr std::size_t MaxLogMsg{2048};

/**
 * Logger callback function.
 */
using LogMsg = StringBuilder<MaxLogMsg>;

/**
 * Logger callback function.
 */
using Logger = void (*)(int, std::string_view);

enum : int {
  /**
   * Critical.
   */
  LogCrit,
  /**
   * Error.
   */
  LogError,
  /**
   * Warning.
   */
  LogWarning,
  /**
   * Notice.
   */
  LogNotice,
  /**
   * Information.
   */
  LogInfo,
  /**
   * Debug.
   */
  LogDebug
};

/**
 * Return log label for given log level.
 */
SWIRLY_API const char* logLabel(int level) noexcept;

/**
 * Return current log level.
 */
SWIRLY_API int getLogLevel() noexcept;

/**
 * Return true if level is less than or equal to current log level.
 */
inline bool isLogLevel(int level) noexcept
{
  return level <= getLogLevel();
}

/**
 * Set log level globally for all threads.
 */
SWIRLY_API int setLogLevel(int level) noexcept;

/**
 * Return current logger.
 */
SWIRLY_API Logger getLogger() noexcept;

/**
 * Set logger globally for all threads.
 */
SWIRLY_API Logger setLogger(Logger logger) noexcept;

/**
 * Unconditionally write log message to the logger. Specifically, this function does not check that
 * level is allowed by the current log level; users are expected to call isLogLevel first, before
 * formatting the log message.
 */
SWIRLY_API void writeLog(int level, std::string_view msg) noexcept;

/**
 * Null logger. This logger does nothing and is effectively /dev/null.
 */
SWIRLY_API void nullLogger(int level, std::string_view msg) noexcept;

/**
 * Standard logger. This logger writes to stdout if the log level is greater than LogWarn, and
 * stdout otherwise.
 */
SWIRLY_API void stdLogger(int level, std::string_view msg) noexcept;

/**
 * System logger. This logger calls syslog().
 */
SWIRLY_API void sysLogger(int level, std::string_view msg) noexcept;

/**
 * Thread-local log message. This thread-local instance of StringBuilder can be used to format log
 * messages before writing to the log. Note that the StringBuilder is reset each time this function
 * is called.
 */
SWIRLY_API LogMsg& logMsg() noexcept;

} // swirly

// N.B. varargs help ensure that expressions such as the following work correctly:
// SWIRLY_LOG(LogInfo, logMsg() << "test: " << Foo<int, int>{10, 20});

#define SWIRLY_LOG(level, ...)                                                                     \
  do {                                                                                             \
    if (swirly::isLogLevel(level))                                                                 \
      swirly::writeLog(level, __VA_ARGS__);                                                        \
  } while (false)

#define SWIRLY_CRIT(...) SWIRLY_LOG(swirly::LogCrit, __VA_ARGS__)
#define SWIRLY_ERROR(...) SWIRLY_LOG(swirly::LogError, __VA_ARGS__)
#define SWIRLY_WARNING(...) SWIRLY_LOG(swirly::LogWarning, __VA_ARGS__)
#define SWIRLY_NOTICE(...) SWIRLY_LOG(swirly::LogNotice, __VA_ARGS__)
#define SWIRLY_INFO(...) SWIRLY_LOG(swirly::LogInfo, __VA_ARGS__)

#if SWIRLY_ENABLE_DEBUG
#define SWIRLY_DEBUG(...) SWIRLY_LOG(swirly::LogDebug, __VA_ARGS__)
#else
#define SWIRLY_DEBUG(...)
#endif

#endif // SWIRLY_ASH_LOG_HPP
