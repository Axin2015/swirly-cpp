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

#include <stdarg.h> // va_list
#include <stdio.h>  // vsnprintf()
#include <string.h> // strcpy()

/**
 * @addtogroup Err
 * @{
 */

enum DbrErrno {
    /**
     * @brief Out of memory.
     */
    DBR_ENOMEM = 1,
    /**
     * @brief Access denied.
     */
    DBR_EACCES,
    /**
     * @brief Invalid argument.
     */
    DBR_EINVAL,
    /**
     * @brief SQL error.
     */
    DBR_EDBSQL,
    /**
     * @brief Is null.
     */
    DBR_ENULL,
    /**
     * @brief User-defined errors must be >=0x400.
     */
    DBR_EUSER = 0x400
};

enum {
    DBR_ERROR_MAX = 255
};

struct DbrErr {
    /**
     * @brief Error number.
     * @sa enum DbrErrno
     */
    int num;
    /**
     * @brief Null terminated error message.
     */
    char msg[DBR_ERROR_MAX + 1];
};

static inline void
dbr_err_clear(struct DbrErr* err)
{
    err->num = 0;
    err->msg[0] = '\0';
}

static inline void
dbr_err_vset(struct DbrErr* err, int num, const char* format, va_list args)
{
    err->num = num;
    const int ret = vsnprintf(err->msg, DBR_ERROR_MAX, format, args);
    // Null termination is _not_ guaranteed by snprintf().
    err->msg[DBR_ERROR_MAX] = '\0';
    if (ret < 0)
        strcpy(err->msg, "bad format");
}

static inline void
dbr_err_set(struct DbrErr* err, int num, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    dbr_err_vset(err, num, format, args);
    va_end(args);
}

/** @} */

#endif // DBR_ERR_H
