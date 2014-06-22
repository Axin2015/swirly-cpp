/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#ifndef DBR_ASH_LOG_H
#define DBR_ASH_LOG_H

/**
 * @file
 * Each thread has its own logger.
 */

#include <dbr/ash/defs.h>

#include <stdarg.h> // va_list

/**
 * @addtogroup Log
 * @{
 */

typedef void (*DbrLogger)(int, const char*);

enum DbrLog {
    /**
     * Critical.
     */
    DBR_LOG_CRIT,
    /**
     * Error.
     */
    DBR_LOG_ERROR,
    /**
     * Warning.
     */
    DBR_LOG_WARN,
    /**
     * Notice.
     */
    DBR_LOG_NOTICE,
    /**
     * Information.
     */
    DBR_LOG_INFO,
    /**
     * Debug level 1.
     */
    DBR_LOG_DEBUG1,
    /**
     * Debug level 2.
     */
    DBR_LOG_DEBUG2,
    /**
     * Debug level 3.
     */
    DBR_LOG_DEBUG3
};

DBR_API const char*
dbr_log_label(int level);

DBR_API int
dbr_log_level(void);

DBR_API DbrLogger
dbr_log_logger(void);

DBR_API int
dbr_log_setlevel(int level);

DBR_API DbrLogger
dbr_log_setlogger(DbrLogger new_logger);

DBR_API void
dbr_log_clnt(int level, const char* msg);

DBR_API void
dbr_log_serv(int level, const char* msg);

DBR_API void
dbr_log_syslog(int level, const char* msg);

DBR_API void
dbr_log_printf(int level, const char* format, ...);

DBR_API void
dbr_log_vprintf(int level, const char* format, va_list args);

#define dbr_log_crit(format, ...)                       \
    dbr_log_printf(DBR_LOG_CRIT, format, ##__VA_ARGS__)
#define dbr_log_error(format, ...)                      \
    dbr_log_printf(DBR_LOG_ERROR, format, ##__VA_ARGS__)
#define dbr_log_warn(format, ...)                       \
    dbr_log_printf(DBR_LOG_WARN, format, ##__VA_ARGS__)
#define dbr_log_notice(format, ...)                     \
    dbr_log_printf(DBR_LOG_NOTICE, format, ##__VA_ARGS__)
#define dbr_log_info(format, ...)                       \
    dbr_log_printf(DBR_LOG_INFO, format, ##__VA_ARGS__)

#if DBR_DEBUG_LEVEL >= 1
#define dbr_log_debug1(format, ...)                     \
    dbr_log_printf(DBR_LOG_DEBUG1, format, ##__VA_ARGS__)
#else  // DBR_DEBUG_LEVEL < 1
#define dbr_log_debug1(format, ...)
#endif // DBR_DEBUG_LEVEL < 1

#if DBR_DEBUG_LEVEL >= 2
#define dbr_log_debug2(format, ...)                     \
    dbr_log_printf(DBR_LOG_DEBUG2, format, ##__VA_ARGS__)
#else  // DBR_DEBUG_LEVEL < 2
#define dbr_log_debug2(format, ...)
#endif // DBR_DEBUG_LEVEL < 2

#if DBR_DEBUG_LEVEL >= 3
#define dbr_log_debug3(format, ...)                     \
    dbr_log_printf(DBR_LOG_DEBUG3, format, ##__VA_ARGS__)
#else  // DBR_DEBUG_LEVEL < 3
#define dbr_log_debug3(format, ...)
#endif // DBR_DEBUG_LEVEL < 3

/** @} */

#endif // DBR_ASH_LOG_H
