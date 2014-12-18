/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_ERR_H
#define DBR_ASH_ERR_H

/**
 * @file
 * Each thread has its own error information.
 */

#include <dbr/ash/defs.h>

#include <stdarg.h> // va_list

/**
 * @addtogroup Err
 * @{
 */

enum DbrErrno {
    /**
     * Interrupted system call.
     */
    DBR_EINTR = 1,
    /**
     * Input/output error.
     */
    DBR_EIO,
    /**
     * Cannot allocate memory.
     */
    DBR_ENOMEM,
    /**
     * Access denied.
     */
    DBR_EACCES,
    /**
     * Device/resource busy.
     */
    DBR_EBUSY,
    /**
     * Already exists.
     */
    DBR_EEXIST,
    /**
     * Invalid argument.
     */
    DBR_EINVAL,
    /**
     * Operation timed-out.
     */
    DBR_ETIMEOUT,
    /**
     * Operating-system error.
     */
    DBR_ESYSTEM,
    /**
     * User-defined errors must be >=0x400.
     */
    DBR_EUSER = 0x400
};

/**
 * Clear last error message.
 */

DBR_API void
dbr_err_clear(void);

/**
 * Print last error with string prefix using logger.
 */

DBR_API void
dbr_err_perror(const char* s);

/**
 * @privatesection
 * @{
 */

DBR_API void
dbr_err_set_(int num, const char* file, int line, const char* msg);

DBR_API void
dbr_err_setf_(int num, const char* file, int line, const char* format, ...);

DBR_API void
dbr_err_vsetf_(int num, const char* file, int line, const char* format, va_list args);

DBR_API void
dbr_err_print_(int num, const char* file, int line, const char* msg);

DBR_API void
dbr_err_printf_(int num, const char* file, int line, const char* format, ...);

DBR_API void
dbr_err_vprintf_(int num, const char* file, int line, const char* format, va_list args);

/** @} */

/**
 * The last error number.
 */

DBR_API int
dbr_err_num(void);

/**
 * The file in which the last error occurred.
 */

DBR_API const char*
dbr_err_file(void);

/**
 * The line at which the last error occurred.
 */

DBR_API int
dbr_err_line(void);

/**
 * The last error message.
 */

DBR_API const char*
dbr_err_msg(void);

/**
 * Set the last error message.
 */
#define dbr_err_set(num, msg)                               \
    dbr_err_set_(num, __FILE__, __LINE__, msg)
/**
 * Set the last error message using printf-style formatting.
 */
#define dbr_err_setf(num, ...)                              \
    dbr_err_setf_(num, __FILE__, __LINE__, __VA_ARGS__)
/**
 * Set the last error message using vprintf-style formatting.
 */
#define dbr_err_vsetf(num, format, args)                    \
    dbr_err_vsetf_(num, __FILE__, __LINE__, format, args)
#define dbr_err_print(num, msg)                             \
    dbr_err_print_(num, __FILE__, __LINE__, msg)
#define dbr_err_printf(num, ...)                            \
    dbr_err_printf_(num, __FILE__, __LINE__, __VA_ARGS__)
#define dbr_err_vprintf(num, format, args)                  \
    dbr_err_vprintf_(num, __FILE__, __LINE__, format, args)

/** @} */

#endif // DBR_ASH_ERR_H
