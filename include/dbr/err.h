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
#include <stdio.h>

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

/** @} */

#if !defined(DBR_HIDE_API)

/**
 * @addtogroup Err
 * @{
 */

DBR_API void
dbr_err_clear(void);

DBR_API void
dbr_err_print(FILE* stream, const char* s);

DBR_API void
dbr_err_vset(int num, const char* format, va_list args);

DBR_API void
dbr_err_set(int num, const char* format, ...);

DBR_API int
dbr_err_num(void);

DBR_API const char*
dbr_err_msg(void);

/** @} */

#endif // !defined(DBR_HIDE_API)

#endif // DBR_ERR_H
