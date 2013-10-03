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
#ifndef TEST_HPP
#define TEST_HPP

#include <dbr/err.h>

#include <cmath>    // fabs()
#include <cstring>  // strcmp()
#include <exception>

class TestException : public std::exception {
    const char* file_;
    int line_;
    char msg_[DBR_ERRMSG_MAX + 1];
public:
    virtual
    ~TestException() noexcept
    {
    }
    TestException(const char* file, int line, const char* msg) noexcept
    {
        file_ = file;
        line_ = line;
        strncpy(msg_, msg, DBR_ERRMSG_MAX);
        msg_[DBR_ERRMSG_MAX] = '\0';
    }
    virtual const char*
    what() const noexcept
    {
        return msg_;
    }
    const char*
    file() const noexcept
    {
        return file_;
    }
    int
    line() const noexcept
    {
        return line_;
    }
};

inline bool
fequal(double lhs, double rhs)
{
    return fabs(lhs - rhs) < 0.0000001;
}

inline bool
sequal(const char* lhs, const char* rhs, size_t n)
{
    return strncmp(lhs, rhs, n) == 0;
}

#define die_(file, line, what)                  \
    throw TestException(file, line, what)

#define die(what)                               \
    die_(__FILE__, __LINE__, what)

#define check(expr)                                     \
    (expr) ? (void)0 : die("check [" #expr "] failed.")

#define TEST_CASE(name) void name(void)

#endif // TEST_HPP
