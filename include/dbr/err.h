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
#ifndef DBR_ERR_H
#define DBR_ERR_H

#include <dbr/defs.h>

#include <stdarg.h> // va_list

/**
 * @addtogroup Err
 * @{
 */

enum DbrErrno {
    /**
     * @brief Interrupted system call.
     */
    DBR_EINTR = 1,
    /**
     * @brief Input/output error.
     */
    DBR_EIO,
    /**
     * @brief Out of memory.
     */
    DBR_ENOMEM,
    /**
     * @brief Access denied.
     */
    DBR_EACCES,
    /**
     * @brief Invalid argument.
     */
    DBR_EINVAL,
    /**
     * @brief User-defined errors must be >=0x400.
     */
    DBR_EUSER = 0x400
};

enum {
    DBR_ERRMSG_MAX = 255
};

/**
 * @brief Clear current error message.
 */

DBR_API void
dbr_err_clear(void);

/**
 * @brief Print current error using logger.
 */

DBR_API void
dbr_err_print(const char* s);

DBR_API void
dbr_err_set_(int num, const char* file, int line, const char* msg);

DBR_API void
dbr_err_setf_(int num, const char* file, int line, const char* format, ...);

DBR_API void
dbr_err_vsetf_(int num, const char* file, int line, const char* format, va_list args);

DBR_API int
dbr_err_num(void);

DBR_API const char*
dbr_err_file(void);

DBR_API int
dbr_err_line(void);

DBR_API const char*
dbr_err_msg(void);

#define dbr_err_set(num, msg)                             \
    dbr_err_set_(num, __FILE__, __LINE__, msg)
#define dbr_err_setf(num, ...)                            \
    dbr_err_setf_(num, __FILE__, __LINE__, __VA_ARGS__)
#define dbr_err_vsetf(num, format, args)                  \
    dbr_err_vsetf_(num, __FILE__, __LINE__, format, args)

/** @} */

#endif // DBR_ERR_H
