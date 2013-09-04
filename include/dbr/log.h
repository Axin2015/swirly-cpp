/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#ifndef DBR_LOG_H
#define DBR_LOG_H

#include <dbr/defs.h>

#include <stdarg.h> // va_list

/**
 * @addtogroup Log
 * @{
 */

typedef void (*DbrLogger)(int, const char*, va_list);

enum DbrLogLevel {
    /**
     * @brief Critical.
     */
    DBR_CRIT,
    /**
     * @brief Error.
     */
    DBR_ERROR,
    /**
     * @brief Warning.
     */
    DBR_WARN,
    /**
     * @brief Notice.
     */
    DBR_NOTICE,
    /**
     * @brief Information.
     */
    DBR_INFO,
    /**
     * @brief Debug level 1.
     */
    DBR_DEBUG1,
    /**
     * @brief Debug level 2.
     */
    DBR_DEBUG2,
    /**
     * @brief Debug level 3.
     */
    DBR_DEBUG3
};

DBR_API DbrLogger
dbr_log_set(DbrLogger new_logger);

DBR_API void
dbr_log_printf(int level, const char* format, ...);

DBR_API void
dbr_log_vprintf(int level, const char* format, va_list args);

#define dbr_log_crit(format, ...)                       \
    dbr_log_printf(DBR_CRIT, format, ##__VA_ARGS__)
#define dbr_log_error(format, ...)                      \
    dbr_log_printf(DBR_ERROR, format, ##__VA_ARGS__)
#define dbr_log_warn(format, ...)                       \
    dbr_log_printf(DBR_WARN, format, ##__VA_ARGS__)
#define dbr_log_notice(format, ...)                     \
    dbr_log_printf(DBR_NOTICE, format, ##__VA_ARGS__)
#define dbr_log_info(format, ...)                       \
    dbr_log_printf(DBR_INFO, format, ##__VA_ARGS__)

#if DBR_DEBUG_LEVEL >= 1
#define dbr_log_debug1(format, ...)                     \
    dbr_log_printf(DBR_DEBUG1, format, ##__VA_ARGS__)
#else  // DBR_DEBUG_LEVEL < 1
#define dbr_log_debug1(format, ...)
#endif // DBR_DEBUG_LEVEL < 1

#if DBR_DEBUG_LEVEL >= 2
#define dbr_log_debug2(format, ...)                     \
    dbr_log_printf(DBR_DEBUG2, format, ##__VA_ARGS__)
#else  // DBR_DEBUG_LEVEL < 2
#define dbr_log_debug2(format, ...)
#endif // DBR_DEBUG_LEVEL < 2

#if DBR_DEBUG_LEVEL >= 3
#define dbr_log_debug3(format, ...)                     \
    dbr_log_printf(DBR_DEBUG3, format, ##__VA_ARGS__)
#else  // DBR_DEBUG_LEVEL < 3
#define dbr_log_debug3(format, ...)
#endif // DBR_DEBUG_LEVEL < 3

/** @} */

#endif // DBR_LOG_H
