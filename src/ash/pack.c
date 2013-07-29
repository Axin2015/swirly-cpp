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
#define _XOPEN_SOURCE 700 // strnlen()
#include <dbr/pack.h>

#include <dbr/err.h>

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Packed integer spec:

// Immediate can store between -64 and 63:
// 00 xxxxxx
// 11 xxxxxx

// Otherwise 10 in high bits signals escape.
// And lower 6 bits contain integer size in bytes:
// 10 000001 0x81
// 10 000010 0x82
// 10 000100 0x84
// 10 001000 0x88

// Escape can be tested using the following expresssion:
// (hdr & '0xc0') == 0x80

// And the following masks-off the lower 6 bits:
// hdr & 3f

enum { MNEM_MAX = 16 };

#if WORDS_BIGENDIAN == 1

static inline int16_t
ntoh16(int16_t i)
{
    return i;
}

static inline int32_t
ntoh32(int32_t i)
{
    return i;
}

static inline int64_t
ntoh64(int64_t i)
{
    return i;
}

static inline int16_t
hton16(int16_t i)
{
    return i;
}

static inline int32_t
hton32(int32_t i)
{
    return i;
}

static inline int64_t
hton64(int64_t i)
{
    return i;
}

#elif WORDS_BIGENDIAN == 0

static inline int16_t
ntoh16(int16_t i)
{
    return __builtin_bswap16(i);
}

static inline int32_t
ntoh32(int32_t i)
{
    return __builtin_bswap32(i);
}

static inline int64_t
ntoh64(int64_t i)
{
    return __builtin_bswap64(i);
}

static inline int16_t
hton16(int16_t i)
{
    return __builtin_bswap16(i);
}

static inline int32_t
hton32(int32_t i)
{
    return __builtin_bswap32(i);
}

static inline int64_t
hton64(int64_t i)
{
    return __builtin_bswap64(i);
}

#else  // WORDS_BIGENDIAN
#error WORDS_BIGENDIAN not defined
#endif // WORDS_BIGENDIAN

static inline void
pack8(char* buf, int8_t i)
{
    *buf = i;
}

static inline void
pack16(char* buf, int16_t i)
{
    i = hton16(i);
    __builtin_memcpy(buf, &i, sizeof(i));
}

static inline void
pack32(char* buf, int32_t i)
{
    i = hton32(i);
    __builtin_memcpy(buf, &i, sizeof(i));
}

static inline void
pack64(char* buf, int64_t i)
{
    i = hton64(i);
    __builtin_memcpy(buf, &i, sizeof(i));
}

static inline int8_t
unpack8(const char* buf)
{
    return *buf;
}

static inline int16_t
unpack16(const char* buf)
{
    int16_t i;
    __builtin_memcpy(&i, buf, sizeof(i));
    return ntoh16(i);
}

static inline int32_t
unpack32(const char* buf)
{
    int32_t i;
    __builtin_memcpy(&i, buf, sizeof(i));
    return ntoh32(i);
}

static inline int64_t
unpack64(const char* buf)
{
    int64_t i;
    __builtin_memcpy(&i, buf, sizeof(i));
    return ntoh64(i);
}

DBR_API size_t
dbr_packleni(int i)
{
    size_t n;
    if (-64 <= i && i <= 63) {
        n = 1;
    } else if (SCHAR_MIN <= i && i <= SCHAR_MAX) {
        n = 2;
    } else if (SHRT_MIN <= i && i <= SHRT_MAX) {
        n = 3;
    } else {
        n = 5;
    }
    return n;
}

DBR_API char*
dbr_packi(char* buf, int i)
{
    if (-64 <= i && i <= 63) {
        pack8(buf, i);
        buf += 1;
    } else if (SCHAR_MIN <= i && i <= SCHAR_MAX) {
        pack8(buf, 0x81);
        buf += 1;
        pack8(buf, i);
        buf += 1;
    } else if (SHRT_MIN <= i && i <= SHRT_MAX) {
        pack8(buf, 0x82);
        buf += 1;
        pack16(buf, i);
        buf += 2;
    } else {
        pack8(buf, 0x84);
        buf += 1;
        pack32(buf, i);
        buf += 4;
    }
    return buf;
}

DBR_API const char*
dbr_unpacki(const char* buf, int* i)
{
    int8_t hdr = unpack8(buf);
    buf += 1;
    if ((hdr & 0xc0) == 0x80) {
        switch (hdr & 0x3f) {
        case 1:
            *i = unpack8(buf);
            buf += 1;
            break;
        case 2:
            *i = unpack16(buf);
            buf += 2;
            break;
        case 4:
            *i = unpack32(buf);
            buf += 4;
            break;
        default:
            dbr_err_set(DBR_EIO, "invalid header 0x%x", (unsigned)hdr);
            buf = NULL;
            break;
        }
    } else
        *i = hdr;
    return buf;
}

DBR_API size_t
dbr_packlenl(long l)
{
    size_t n;
    if (-64 <= l && l <= 63) {
        n = 1;
    } else if (SCHAR_MIN <= l && l <= SCHAR_MAX) {
        n = 2;
    } else if (SHRT_MIN <= l && l <= SHRT_MAX) {
        n = 3;
    } else if (INT_MIN <= l && l <= INT_MAX) {
        n = 5;
    } else {
        n = 9;
    }
    return n;
}

DBR_API char*
dbr_packl(char* buf, long l)
{
    if (-64 <= l && l <= 63) {
        pack8(buf, l);
        buf += 1;
    } else if (SCHAR_MIN <= l && l <= SCHAR_MAX) {
        pack8(buf, 0x81);
        buf += 1;
        pack8(buf, l);
        buf += 1;
    } else if (SHRT_MIN <= l && l <= SHRT_MAX) {
        pack8(buf, 0x82);
        buf += 1;
        pack16(buf, l);
        buf += 2;
    } else if (INT_MIN <= l && l <= INT_MAX) {
        pack8(buf, 0x84);
        buf += 1;
        pack32(buf, l);
        buf += 4;
    } else {
        pack8(buf, 0x88);
        buf += 1;
        pack64(buf, l);
        buf += 8;
    }
    return buf;
}

DBR_API const char*
dbr_unpackl(const char* buf, long* l)
{
    int8_t hdr = unpack8(buf);
    buf += 1;
    if ((hdr & 0xc0) == 0x80) {
        switch (hdr & 0x3f) {
        case 1:
            *l = unpack8(buf);
            buf += 1;
            break;
        case 2:
            *l = unpack16(buf);
            buf += 2;
            break;
        case 4:
            *l = unpack32(buf);
            buf += 4;
            break;
        case 8:
            *l = unpack64(buf);
            buf += 8;
            break;
        default:
            dbr_err_set(DBR_EIO, "invalid header 0x%x", (unsigned)hdr);
            buf = NULL;
            break;
        }
    } else
        *l = hdr;
    return buf;
}

DBR_API size_t
dbr_packlens(const char* s, size_t m)
{
    const size_t n = strnlen(s, m);
    return dbr_packlenz(n) + n;
}

DBR_API char*
dbr_packs(char* buf, const char* s, size_t m)
{
    const size_t n = strnlen(s, m);
    buf = dbr_packz(buf, n);
    __builtin_memcpy(buf, s, n);
    return buf + n;
}

DBR_API const char*
dbr_unpacks(const char* buf, char* s, size_t m)
{
    size_t n;
    // Unpack string length.
    if ((buf = dbr_unpackz(buf, &n))) {
        if (n < m) {
            // Null terminate when less than max.
            __builtin_memcpy(s, buf, n);
            s[n] = '\0';
        } else
            __builtin_memcpy(s, buf, m);
        buf += n;
    }
    return buf;
}

DBR_API size_t
dbr_packlenf(const char* format, ...)
{
    size_t n;
    va_list args;
    va_start(args, format);
    n = dbr_vpacklenf(format, args);
    va_end(args);
    return n;
}

DBR_API char*
dbr_packf(char* buf, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    buf = dbr_vpackf(buf, format, args);
    va_end(args);
    return buf;
}

DBR_API const char*
dbr_unpackf(const char* buf, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    buf = dbr_vunpackf(buf, format, args);
    va_end(args);
    return buf;
}

DBR_API size_t
dbr_vpacklenf(const char* format, va_list args)
{
    size_t n = 0;
    for (const char* cp = format; *cp != '\0'; ++cp) {
        size_t m;
        const char* s;
        switch (*cp) {
        case 'd':
        case 'i':
            n += dbr_packleni(va_arg(args, int));
            break;
        case 'l':
            n += dbr_packlenl(va_arg(args, long));
            break;
        case 'z':
            n += dbr_packlenz(va_arg(args, size_t));
            break;
        case 'm':
            s = va_arg(args, const char*);
            n += dbr_packlens(s, MNEM_MAX);
            break;
        case 's':
            m = va_arg(args, size_t);
            s = va_arg(args, const char*);
            n += dbr_packlens(s, m);
            break;
        default:
            abort();
        }
    }
    return n;
}

DBR_API char*
dbr_vpackf(char* buf, const char* format, va_list args)
{
    for (const char* cp = format; *cp != '\0'; ++cp) {
        size_t m;
        const char* s;
        switch (*cp) {
        case 'd':
        case 'i':
            buf = dbr_packi(buf, va_arg(args, int));
            break;
        case 'l':
            buf = dbr_packl(buf, va_arg(args, long));
            break;
        case 'z':
            buf = dbr_packz(buf, va_arg(args, size_t));
            break;
        case 'm':
            s = va_arg(args, const char*);
            buf = dbr_packs(buf, s, MNEM_MAX);
            break;
        case 's':
            m = va_arg(args, size_t);
            s = va_arg(args, const char*);
            buf = dbr_packs(buf, s, m);
            break;
        default:
            abort();
        }
    }
    return buf;
}

DBR_API const char*
dbr_vunpackf(const char* buf, const char* format, va_list args)
{
    for (const char* cp = format; buf && *cp != '\0'; ++cp) {
        size_t m;
        char* s;
        switch (*cp) {
        case 'd':
        case 'i':
            buf = dbr_unpacki(buf, va_arg(args, int*));
            break;
        case 'l':
            buf = dbr_unpackl(buf, va_arg(args, long*));
            break;
        case 'z':
            buf = dbr_unpackz(buf, va_arg(args, size_t*));
            break;
        case 'm':
            s = va_arg(args, char*);
            buf = dbr_unpacks(buf, s, MNEM_MAX);
            break;
        case 's':
            m = va_arg(args, size_t);
            s = va_arg(args, char*);
            buf = dbr_unpacks(buf, s, m);
            break;
        default:
            abort();
        }
    }
    return buf;
}
