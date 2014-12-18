/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef TEST_HPP
#define TEST_HPP

#include <sc/ash/err.h>

#include <cmath>    // fabs()
#include <cstring>  // strcmp()
#include <exception>

class TestException : public std::exception {
    const char* file_;
    int line_;
    char msg_[SC_ERRMSG_MAX + 1];
public:
    virtual
    ~TestException() noexcept
    {
    }
    TestException(const char* file, int line, const char* msg) noexcept
    {
        file_ = file;
        line_ = line;
        strncpy(msg_, msg, SC_ERRMSG_MAX);
        msg_[SC_ERRMSG_MAX] = '\0';
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
