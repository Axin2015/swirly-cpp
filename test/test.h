/**
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
#ifndef TEST_H
#define TEST_H

#include <math.h>    // fabs()
#include <stdbool.h> // bool
#include <stdio.h>   // fprintf()
#include <stdlib.h>  // exit()
#include <string.h>  // strcmp()

#define die_(file, line, what)                                          \
    (fprintf(stderr, "%s:%d: %s\n", file, line, what), fflush(NULL), exit(1))

#define die(what)                               \
    die_(__FILE__, __LINE__, what)

#define check(expr)                                     \
    (expr) ? (void)0 : die("check [" #expr "] failed.")

static inline bool
fequal(double lhs, double rhs)
{
    return fabs(lhs - rhs) < 0.0000001;
}

static inline bool
sequal(const char* lhs, const char* rhs, size_t n)
{
    return strncmp(lhs, rhs, n) == 0;
}

#endif // TEST_H
