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
#ifndef DBR_EXCEPT_HPP
#define DBR_EXCEPT_HPP

#include <dbr/err.h>

#include <cstring>
#include <stdexcept>

namespace dbr {

class DbrException : public std::exception {
    int num_;
    char msg_[DBR_ERROR_MAX + 1];
public:
    virtual
    ~DbrException() noexcept
    {
    }
    DbrException(int num, const char* msg) noexcept
    {
        num_ = num;
        strncpy(msg_, msg, DBR_ERROR_MAX);
        msg_[DBR_ERROR_MAX] = '\0';
    }
    DbrException() noexcept
    : DbrException(dbr_err_num(), dbr_err_msg())
    {
    }
    virtual const char*
    what() const noexcept
    {
        return msg_;
    }
    int
    num() const noexcept
    {
        return num_;
    }
};

template <int E>
class BasicException : public DbrException {
public:
    virtual
    ~BasicException() noexcept
    {
    }
    BasicException(const char* errmsg) noexcept
    : DbrException(E, errmsg)
    {
    }
};

typedef BasicException<DBR_ENOMEM> NoMemException;
typedef BasicException<DBR_EACCES> AccesException;
typedef BasicException<DBR_EINVAL> InvalException;
typedef BasicException<DBR_EDBSQL> DbSqlException;
typedef BasicException<DBR_ENULL> NullException;

inline void
throw_exception(int num, const char* msg)
{
    switch (num) {
    case DBR_ENOMEM:
        throw NoMemException(msg);
        break;
    case DBR_EACCES:
        throw AccesException(msg);
        break;
    case DBR_EINVAL:
        throw InvalException(msg);
        break;
    case DBR_EDBSQL:
        throw DbSqlException(msg);
        break;
    case DBR_ENULL:
        throw NullException(msg);
        break;
    default:
        throw DbrException(num, msg);
        break;
    }
}

inline void
throw_exception()
{
    throw_exception(dbr_err_num(), dbr_err_msg());
}
} // dbr

#endif // DBR_EXCEPT_HPP
