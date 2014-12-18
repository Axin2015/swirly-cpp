/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_PACK_H
#define SC_ASH_PACK_H

#include <sc/ash/defs.h>

#include <stdarg.h>
#include <stddef.h> // size_t

/**
 * @addtogroup Pack
 * @{
 */

SC_API size_t
sc_pack_leni(int i);

SC_API char*
sc_packi(char* buf, int i);

/**
 * @return NULL on error.
 */

SC_API const char*
sc_unpacki(const char* buf, int* i);

SC_API size_t
sc_pack_lenl(long l);

SC_API char*
sc_packl(char* buf, long l);

/**
 * @return NULL on error.
 */

SC_API const char*
sc_unpackl(const char* buf, long* l);

static inline size_t
sc_pack_lenz(size_t z)
{
    return sc_pack_lenl(z);
}

static inline char*
sc_packz(char* buf, size_t z)
{
    return sc_packl(buf, z);
}

/**
 * @return NULL on error.
 */

static inline const char*
sc_unpackz(const char* buf, size_t* z)
{
    long n;
    if ((buf = sc_unpackl(buf, &n)))
        *z = n;
    return buf;
}

SC_API size_t
sc_pack_lens(const char* s, size_t m);

SC_API char*
sc_packs(char* buf, const char* s, size_t m);

/**
 * @return NULL on error.
 */

SC_API const char*
sc_unpacks(const char* buf, char* s, size_t m);

SC_API size_t
sc_pack_lenf(const char* format, ...);

SC_API char*
sc_packf(char* buf, const char* format, ...);

/**
 * @return NULL on error.
 */

SC_API const char*
sc_unpackf(const char* buf, const char* format, ...);

SC_API size_t
sc_vpack_lenf(const char* format, va_list args);

SC_API char*
sc_vpackf(char* buf, const char* format, va_list args);

/**
 * @return NULL on error.
 */

SC_API const char*
sc_vunpackf(const char* buf, const char* format, va_list args);

/** @} */

#endif // SC_ASH_PACK_H
