/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_ERR_H
#define SC_ASH_ERR_H

/**
 * @file
 * Each thread has its own error information.
 */

#include <sc/ash/defs.h>

#include <stdarg.h> // va_list

/**
 * @addtogroup Err
 * @{
 */

enum ScErrno {
    /**
     * Interrupted system call.
     */
    SC_EINTR = 1,
    /**
     * Input/output error.
     */
    SC_EIO,
    /**
     * Cannot allocate memory.
     */
    SC_ENOMEM,
    /**
     * Access denied.
     */
    SC_EACCES,
    /**
     * Device/resource busy.
     */
    SC_EBUSY,
    /**
     * Already exists.
     */
    SC_EEXIST,
    /**
     * Invalid argument.
     */
    SC_EINVAL,
    /**
     * Operation timed-out.
     */
    SC_ETIMEOUT,
    /**
     * Operating-system error.
     */
    SC_ESYSTEM,
    /**
     * User-defined errors must be >=0x400.
     */
    SC_EUSER = 0x400
};

/**
 * Clear last error message.
 */

SC_API void
sc_err_clear(void);

/**
 * Print last error with string prefix using logger.
 */

SC_API void
sc_err_perror(const char* s);

/**
 * @privatesection
 * @{
 */

SC_API void
sc_err_set_(int num, const char* file, int line, const char* msg);

SC_API void
sc_err_setf_(int num, const char* file, int line, const char* format, ...);

SC_API void
sc_err_vsetf_(int num, const char* file, int line, const char* format, va_list args);

SC_API void
sc_err_print_(int num, const char* file, int line, const char* msg);

SC_API void
sc_err_printf_(int num, const char* file, int line, const char* format, ...);

SC_API void
sc_err_vprintf_(int num, const char* file, int line, const char* format, va_list args);

/** @} */

/**
 * The last error number.
 */

SC_API int
sc_err_num(void);

/**
 * The file in which the last error occurred.
 */

SC_API const char*
sc_err_file(void);

/**
 * The line at which the last error occurred.
 */

SC_API int
sc_err_line(void);

/**
 * The last error message.
 */

SC_API const char*
sc_err_msg(void);

/**
 * Set the last error message.
 */
#define sc_err_set(num, msg)                               \
    sc_err_set_(num, __FILE__, __LINE__, msg)
/**
 * Set the last error message using printf-style formatting.
 */
#define sc_err_setf(num, ...)                              \
    sc_err_setf_(num, __FILE__, __LINE__, __VA_ARGS__)
/**
 * Set the last error message using vprintf-style formatting.
 */
#define sc_err_vsetf(num, format, args)                    \
    sc_err_vsetf_(num, __FILE__, __LINE__, format, args)
#define sc_err_print(num, msg)                             \
    sc_err_print_(num, __FILE__, __LINE__, msg)
#define sc_err_printf(num, ...)                            \
    sc_err_printf_(num, __FILE__, __LINE__, __VA_ARGS__)
#define sc_err_vprintf(num, format, args)                  \
    sc_err_vprintf_(num, __FILE__, __LINE__, format, args)

/** @} */

#endif // SC_ASH_ERR_H
