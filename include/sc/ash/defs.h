/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_DEFS_H
#define SC_ASH_DEFS_H

#if !defined(__cplusplus)
# define SC_EXTERN extern
#else  // defined(__cplusplus)
# define restrict __restrict__
# define SC_EXTERN extern "C"
#endif // defined(__cplusplus)

#define SC_API SC_EXTERN __attribute__((visibility ("default")))

/**
 * @addtogroup Simple
 * @{
 */

enum {
    /**
     * Maximum length of error messages.
     */
    SC_ERRMSG_MAX = 128,
    /**
     * Maximum length of log messages.
     */
    SC_LOGMSG_MAX = 512,
    /**
     * A UUID is represented by 32 lowercase hexadecimal digits, displayed in five groups separated
     * by hyphens.
     */
    SC_UUID_MAX = 36
};

enum {
    /**
     * Boolean logical false.
     */
    SC_FALSE,
    /**
     * Boolean logical true.
     */
    SC_TRUE
};

/**
 * Boolean logic type.
 */
typedef int ScBool;
/**
 * Identifier type.
 */
typedef long ScIden;
/**
 * Key type used by associative containers.
 * @sa struct RbNode
 */
typedef unsigned long ScKey;
/**
 * Universally unique identifier.
 */
typedef unsigned char ScUuid[16];

/** @} */

/**
 * @addtogroup Util
 * @{
 */

/**
 * @return the smallest of @a a and @a b. If both are equivalent, @a a is returned.
 */
#define sc_min(a, b) ((a) <= (b) ? (a) : (b))

/**
 * @return the largest of @a a and @a b. If both are equivalent, @a a is returned.
 */
#define sc_max(a, b) ((a) >= (b) ? (a) : (b))

/**
 * Expected branch.
 */
#if !defined(__GNUC__)
#define sc_likely(x)   (x)
#else  // defined(__GNUC__)
#define sc_likely(x)   __builtin_expect(!!(x), 1)
#endif // defined(__GNUC__)

/**
 * Unexpected branch.
 */
#if !defined(__GNUC__)
#define sc_unlikely(x) (x)
#else  // defined(__GNUC__)
#define sc_unlikely(x) __builtin_expect(!!(x), 0)
#endif // defined(__GNUC__)

/**
 * http://en.wikipedia.org/wiki/Offsetof
 */
#define sc_offsetof(st, m) __builtin_offsetof(st, m)
#define sc_implof(s, m, p) (s*)((char*)(p) - sc_offsetof(s, m))

// FIXME: Review memory barrier usage.
#define sc_wmb() do { } while (0)
#define sc_rmb() do { } while (0)

/** @} */

#endif // SC_ASH_DEFS_H
