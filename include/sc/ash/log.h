/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_LOG_H
#define SC_ASH_LOG_H

/**
 * @file
 * Each thread has its own logger.
 */

#include <sc/ash/defs.h>

#include <stdarg.h> // va_list

/**
 * @addtogroup Log
 * @{
 */

typedef void (*ScLogger)(int, const char*);

enum ScLog {
    /**
     * Critical.
     */
    SC_LOG_CRIT,
    /**
     * Error.
     */
    SC_LOG_ERROR,
    /**
     * Warning.
     */
    SC_LOG_WARN,
    /**
     * Notice.
     */
    SC_LOG_NOTICE,
    /**
     * Information.
     */
    SC_LOG_INFO,
    /**
     * Debug level 1.
     */
    SC_LOG_DEBUG1,
    /**
     * Debug level 2.
     */
    SC_LOG_DEBUG2,
    /**
     * Debug level 3.
     */
    SC_LOG_DEBUG3
};

SC_API const char*
sc_log_label(int level);

SC_API int
sc_log_level(void);

SC_API ScLogger
sc_log_logger(void);

SC_API int
sc_log_setlevel(int level);

SC_API ScLogger
sc_log_setlogger(ScLogger new_logger);

SC_API void
sc_log_clnt(int level, const char* msg);

SC_API void
sc_log_serv(int level, const char* msg);

SC_API void
sc_log_syslog(int level, const char* msg);

SC_API void
sc_log_printf(int level, const char* format, ...);

SC_API void
sc_log_vprintf(int level, const char* format, va_list args);

#define sc_log_crit(format, ...)                       \
    sc_log_printf(SC_LOG_CRIT, format, ##__VA_ARGS__)
#define sc_log_error(format, ...)                      \
    sc_log_printf(SC_LOG_ERROR, format, ##__VA_ARGS__)
#define sc_log_warn(format, ...)                       \
    sc_log_printf(SC_LOG_WARN, format, ##__VA_ARGS__)
#define sc_log_notice(format, ...)                     \
    sc_log_printf(SC_LOG_NOTICE, format, ##__VA_ARGS__)
#define sc_log_info(format, ...)                       \
    sc_log_printf(SC_LOG_INFO, format, ##__VA_ARGS__)

#if SC_DEBUG_LEVEL >= 1
#define sc_log_debug1(format, ...)                     \
    sc_log_printf(SC_LOG_DEBUG1, format, ##__VA_ARGS__)
#else  // SC_DEBUG_LEVEL < 1
#define sc_log_debug1(format, ...)
#endif // SC_DEBUG_LEVEL < 1

#if SC_DEBUG_LEVEL >= 2
#define sc_log_debug2(format, ...)                     \
    sc_log_printf(SC_LOG_DEBUG2, format, ##__VA_ARGS__)
#else  // SC_DEBUG_LEVEL < 2
#define sc_log_debug2(format, ...)
#endif // SC_DEBUG_LEVEL < 2

#if SC_DEBUG_LEVEL >= 3
#define sc_log_debug3(format, ...)                     \
    sc_log_printf(SC_LOG_DEBUG3, format, ##__VA_ARGS__)
#else  // SC_DEBUG_LEVEL < 3
#define sc_log_debug3(format, ...)
#endif // SC_DEBUG_LEVEL < 3

/** @} */

#endif // SC_ASH_LOG_H
