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
#ifndef SWIRLY_UTIL_LOG_HPP
#define SWIRLY_UTIL_LOG_HPP

#include <swirly/util/Limits.hpp>
#include <swirly/util/Stream.hpp>

namespace swirly {
inline namespace util {

/**
 * Logger callback function.
 */
using Logger = void (*)(int, std::string_view);

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
 * Logger callback function.
 */
using LogMsg = StaticStream<MaxLogMsg>;

/**
 * Thread-local log message. This thread-local instance of StaticStream can be used to format log
 * messages before writing to the log. Note that the StaticStream is reset each time this function
 * is called.
 */
SWIRLY_API LogMsg& logMsg() noexcept;

// Inspired by techniques developed by Rodrigo Fernandes.
class Log {
    template <typename ValueT>
    friend Log& operator<<(Log& log, const ValueT& val);

    template <typename ValueT>
    friend Log& operator<<(Log&& log, const ValueT& val);

  public:
    enum : int {
        /**
     * Critical.
     */
        Crit,
        /**
     * Error.
     */
        Error,
        /**
     * Warning.
     */
        Warning,
        /**
     * Notice.
     */
        Notice,
        /**
     * Information.
     */
        Info,
        /**
     * Debug.
     */
        Debug
    };

    explicit Log(int level) noexcept
    : level_{level}
    , msg_(logMsg())
    {
    }
    ~Log() { writeLog(level_, msg_); }

    // Copy.
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    // Move.
    Log(Log&&) = delete;
    Log& operator=(Log&&) = delete;

    constexpr explicit operator bool() const { return true; }

  private:
    const int level_;
    LogMsg& msg_;
};

template <typename ValueT>
Log& operator<<(Log& log, const ValueT& val)
{
    log.msg_ << val;
    return log;
}

template <typename ValueT>
Log& operator<<(Log&& log, const ValueT& val)
{
    return log << val;
}

} // namespace util
} // namespace swirly

// clang-format off
#define SWIRLY_LOG(level) swirly::isLogLevel(level) && swirly::Log{level}

#define SWIRLY_CRIT SWIRLY_LOG(swirly::Log::Crit)
#define SWIRLY_ERROR SWIRLY_LOG(swirly::Log::Error)
#define SWIRLY_WARNING SWIRLY_LOG(swirly::Log::Warning)
#define SWIRLY_NOTICE SWIRLY_LOG(swirly::Log::Notice)
#define SWIRLY_INFO SWIRLY_LOG(swirly::Log::Info)

#if SWIRLY_ENABLE_DEBUG
#define SWIRLY_DEBUG SWIRLY_LOG(swirly::Log::Debug)
#else
#define SWIRLY_DEBUG false && swirly::Log{swirly::Log::Debug}
#endif
// clang-format on

#endif // SWIRLY_UTIL_LOG_HPP
