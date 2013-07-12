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
#ifndef DBR_DEFS_H
#define DBR_DEFS_H

#if !defined(__cplusplus)
# define DBR_EXTERN extern
#else  // __cplusplus
# define restrict __restrict__
# define DBR_EXTERN extern "C"
#endif // __cplusplus

#define DBR_API DBR_EXTERN __attribute__((visibility ("default")))

/**
 * @addtogroup Types
 * @{
 */

typedef int DbrBool;

enum {
    DBR_FALSE,
    DBR_TRUE
};

/** @} */

/**
 * @addtogroup Util
 * @{
 */

#if DBR_DEBUG_LEVEL >= 1
#include <stdio.h>
#define DBR_DEBUG1(fmt)                         \
    printf(fmt "\n")
#define DBR_DEBUG1F(fmt, ...)                   \
    printf(fmt "\n", __VA_ARGS__)
#define DBR_CHECKPOINT                                      \
    DBR_DEBUG1F("%s:%d: checkpoint", __FILE__, __LINE__)
#else  // DBR_DEBUG_LEVEL < 1
#define DBR_DEBUG1(fmt)
#define DBR_DEBUG1F(fmt, ...)
#define DBR_CHECKPOINT
#endif // DBR_DEBUG_LEVEL < 1

#if DBR_DEBUG_LEVEL >= 2
#include <stdio.h>
#define DBR_DEBUG2(fmt)                         \
    printf(fmt "\n")
#define DBR_DEBUG2F(fmt, ...)                   \
    printf(fmt "\n", __VA_ARGS__)
#else  // DBR_DEBUG_LEVEL < 2
#define DBR_DEBUG2(fmt)
#define DBR_DEBUG2F(fmt, ...)
#endif // DBR_DEBUG_LEVEL < 2

#if DBR_DEBUG_LEVEL >= 3
#include <stdio.h>
#define DBR_DEBUG3(fmt)                         \
    printf(fmt "\n")
#define DBR_DEBUG3F(fmt, ...)                   \
    printf(fmt "\n", __VA_ARGS__)
#else  // DBR_DEBUG_LEVEL < 3
#define DBR_DEBUG3(fmt)
#define DBR_DEBUG3F(fmt, ...)
#endif // DBR_DEBUG_LEVEL < 3

#define dbr_min(a, b) ((a) <= (b) ? (a) : (b))
#define dbr_max(a, b) ((a) >= (b) ? (a) : (b))

#if !defined(__GNUC__)
#define dbr_likely(x)   (x)
#define dbr_unlikely(x) (x)
#else  // __GNUC__
#define dbr_likely(x)   __builtin_expect(!!(x), 1)
#define dbr_unlikely(x) __builtin_expect(!!(x), 0)
#endif // __GNUC__

// http://en.wikipedia.org/wiki/Offsetof

#define dbr_offsetof(st, m) __builtin_offsetof(st, m)
#define dbr_implof(s, m, p) (s*)((char*)(p) - dbr_offsetof(s, m))

/** @} */

#endif // DBR_DEFS_H
