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

#include <string.h>

enum { MNEM_MAX = 16 };

#if WORDS_BIGENDIAN == 1

static inline uint16_t
ntoh16(uint16_t i)
{
    return i;
}

static inline uint32_t
ntoh32(uint32_t i)
{
    return i;
}

static inline uint64_t
ntoh64(uint64_t i)
{
    return i;
}

static inline uint16_t
hton16(uint16_t i)
{
    return i;
}

static inline uint32_t
hton32(uint32_t i)
{
    return i;
}

static inline uint64_t
hton64(uint64_t i)
{
    return i;
}

#elif WORDS_BIGENDIAN == 0

static inline uint16_t
ntoh16(uint16_t i)
{
    return __builtin_bswap16(i);
}

static inline uint32_t
ntoh32(uint32_t i)
{
    return __builtin_bswap32(i);
}

static inline uint64_t
ntoh64(uint64_t i)
{
    return __builtin_bswap64(i);
}

static inline uint16_t
hton16(uint16_t i)
{
    return __builtin_bswap16(i);
}

static inline uint32_t
hton32(uint32_t i)
{
    return __builtin_bswap32(i);
}

static inline uint64_t
hton64(uint64_t i)
{
    return __builtin_bswap64(i);
}

#else  // WORDS_BIGENDIAN
#error WORDS_BIGENDIAN not defined
#endif // WORDS_BIGENDIAN

DBR_API char*
dbr_pack_int8(char* buf, int8_t i)
{
    *buf = i;
    return buf + sizeof(i);
}

DBR_API char*
dbr_pack_uint8(char* buf, uint8_t i)
{
    *buf = i;
    return buf + sizeof(i);
}

DBR_API char*
dbr_pack_int16(char* buf, int16_t i)
{
    i = hton16(i);
    __builtin_memcpy(buf, &i, sizeof(i));
    return buf + sizeof(i);
}

DBR_API char*
dbr_pack_uint16(char* buf, uint16_t i)
{
    i = hton16(i);
    __builtin_memcpy(buf, &i, sizeof(i));
    return buf + sizeof(i);
}

DBR_API char*
dbr_pack_int32(char* buf, int32_t i)
{
    i = hton32(i);
    __builtin_memcpy(buf, &i, sizeof(i));
    return buf + sizeof(i);
}

DBR_API char*
dbr_pack_uint32(char* buf, uint32_t i)
{
    i = hton32(i);
    __builtin_memcpy(buf, &i, sizeof(i));
    return buf + sizeof(i);
}

DBR_API char*
dbr_pack_int64(char* buf, int64_t i)
{
    i = hton64(i);
    __builtin_memcpy(buf, &i, sizeof(i));
    return buf + sizeof(i);
}

DBR_API char*
dbr_pack_uint64(char* buf, uint64_t i)
{
    i = hton64(i);
    __builtin_memcpy(buf, &i, sizeof(i));
    return buf + sizeof(i);
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

DBR_API char*
dbr_vpackf(char* buf, const char* format, va_list args)
{
    for (const char* cp = format; *cp != '\0'; ++cp) {
        uint32_t n;
        const char* s;
        switch (*cp) {
        case 'i':
            buf = dbr_pack_int32(buf, va_arg(args, int32_t));
            break;
        case 'I':
            buf = dbr_pack_uint32(buf, va_arg(args, uint32_t));
            break;
        case 'l':
            buf = dbr_pack_int64(buf, va_arg(args, int64_t));
            break;
        case 'L':
            buf = dbr_pack_uint64(buf, va_arg(args, uint64_t));
            break;
        case 'm':
            s = va_arg(args, const char*);
            n = strnlen(s, MNEM_MAX);
            buf = dbr_pack_uint16(buf, n);
            __builtin_memcpy(buf, s, n);
            buf += n;
            break;
        case 's':
            n = va_arg(args, uint32_t);
            s = va_arg(args, const char*);
            n = strnlen(s, n);
            buf = dbr_pack_uint16(buf, n);
            __builtin_memcpy(buf, s, n);
            buf += n;
            break;
        default:
            dbr_err_set(DBR_EINVAL, "invalid format character '%c'", *cp);
            return NULL;
        }
    }
    return buf;
}

DBR_API const char*
dbr_unpack_int8(const char* buf, int8_t* i)
{
    *i = *buf;
    return buf + sizeof(*i);
}

DBR_API const char*
dbr_unpack_uint8(const char* buf, uint8_t* i)
{
    *i = *buf;
    return buf + sizeof(*i);
}

DBR_API const char*
dbr_unpack_int16(const char* buf, int16_t* i)
{
    __builtin_memcpy(i, buf, sizeof(*i));
    *i = ntoh16(*i);
    return buf + sizeof(*i);
}

DBR_API const char*
dbr_unpack_uint16(const char* buf, uint16_t* i)
{
    __builtin_memcpy(i, buf, sizeof(*i));
    *i = ntoh16(*i);
    return buf + sizeof(*i);
}

DBR_API const char*
dbr_unpack_int32(const char* buf, int32_t* i)
{
    __builtin_memcpy(i, buf, sizeof(*i));
    *i = ntoh32(*i);
    return buf + sizeof(*i);
}

DBR_API const char*
dbr_unpack_uint32(const char* buf, uint32_t* i)
{
    __builtin_memcpy(i, buf, sizeof(*i));
    *i = ntoh32(*i);
    return buf + sizeof(*i);
}

DBR_API const char*
dbr_unpack_int64(const char* buf, int64_t* i)
{
    __builtin_memcpy(i, buf, sizeof(*i));
    *i = ntoh64(*i);
    return buf + sizeof(*i);
}

DBR_API const char*
dbr_unpack_uint64(const char* buf, uint64_t* i)
{
    __builtin_memcpy(i, buf, sizeof(*i));
    *i = ntoh64(*i);
    return buf + sizeof(*i);
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

DBR_API const char*
dbr_vunpackf(const char* buf, const char* format, va_list args)
{
    for (const char* cp = format; *cp != '\0'; ++cp) {
        uint32_t n;
        uint16_t m;
        char* s;
        switch (*cp) {
        case 'i':
            buf = dbr_unpack_int32(buf, va_arg(args, int32_t*));
            break;
        case 'I':
            buf = dbr_unpack_uint32(buf, va_arg(args, uint32_t*));
            break;
        case 'l':
            buf = dbr_unpack_int64(buf, va_arg(args, int64_t*));
            break;
        case 'L':
            buf = dbr_unpack_uint64(buf, va_arg(args, uint64_t*));
            break;
        case 'm':
            s = va_arg(args, char*);
            buf = dbr_unpack_uint16(buf, &m);
            __builtin_memcpy(s, buf, dbr_min(MNEM_MAX, m));
            buf += dbr_min(MNEM_MAX, m);
            break;
        case 's':
            n = va_arg(args, uint32_t);
            s = va_arg(args, char*);
            buf = dbr_unpack_uint16(buf, &m);
            __builtin_memcpy(s, buf, dbr_min(n, m));
            buf += dbr_min(n, m);
            break;
        default:
            dbr_err_set(DBR_EINVAL, "invalid format character '%c'", *cp);
            return NULL;
        }
    }
    return buf;
}
