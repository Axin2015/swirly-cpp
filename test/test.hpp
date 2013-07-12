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

#include <dbrpp/except.hpp>

#include <cmath>    // fabs()
#include <cstring>  // strcmp()

#define die_(file, line, what)                                          \
    throw dbrpp::AssrtException(file, line, what)

#define die(what)                               \
    die_(__FILE__, __LINE__, what)

#define check(expr)                                     \
    (expr) ? (void)0 : die("check [" #expr "] failed.")

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

#define TEST_CASE(name) void name(void)

#endif // TEST_HPP
