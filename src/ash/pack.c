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
dbr_pack16(char* buf, uint16_t i)
{
    i = hton16(i);
    __builtin_memcpy(buf, &i, sizeof(i));
    return buf + sizeof(i);
}

DBR_API char*
dbr_pack32(char* buf, uint32_t i)
{
    i = hton32(i);
    __builtin_memcpy(buf, &i, sizeof(i));
    return buf + sizeof(i);
}

DBR_API char*
dbr_pack64(char* buf, uint64_t i)
{
    i = hton64(i);
    __builtin_memcpy(buf, &i, sizeof(i));
    return buf + sizeof(i);
}

DBR_API char*
dbr_pack(char* buf, const char* format, va_list args)
{
    for (const char* cp = format; *cp != '\0'; ++cp) {
        uint32_t i;
        uint64_t l;
        const char* s;
        switch (*cp) {
        case 'd':
        case 'i':
            i = va_arg(args, uint32_t);
            buf = dbr_pack32(buf, i);
            break;
        case 'l':
            l = va_arg(args, uint64_t);
            buf = dbr_pack64(buf, l);
            break;
        case 'm':
            s = va_arg(args, const char*);
            i = strnlen(s, MNEM_MAX);
            buf = dbr_pack16(buf, i);
            __builtin_memcpy(buf, s, i);
            buf += i;
            break;
        case 's':
            i = va_arg(args, uint32_t);
            s = va_arg(args, const char*);
            i = strnlen(s, i);
            buf = dbr_pack16(buf, i);
            __builtin_memcpy(buf, s, i);
            buf += i;
            break;
        default:
            dbr_err_set(DBR_EINVAL, "invalid format character '%c'", *cp);
            return NULL;
        }
    }
    return buf;
}

DBR_API const char*
dbr_unpack16(const char* buf, uint16_t* i)
{
    __builtin_memcpy(i, buf, sizeof(*i));
    *i = ntoh16(*i);
    return buf + sizeof(*i);
}

DBR_API const char*
dbr_unpack32(const char* buf, uint32_t* i)
{
    __builtin_memcpy(i, buf, sizeof(*i));
    *i = ntoh32(*i);
    return buf + sizeof(*i);
}

DBR_API const char*
dbr_unpack64(const char* buf, uint64_t* i)
{
    __builtin_memcpy(i, buf, sizeof(*i));
    *i = ntoh64(*i);
    return buf + sizeof(*i);
}

DBR_API const char*
dbr_unpack(const char* buf, const char* format, va_list args)
{
    for (const char* cp = format; *cp != '\0'; ++cp) {
        uint16_t n;
        uint32_t* i, j;
        uint64_t* l;
        char* s;
        switch (*cp) {
        case 'd':
        case 'i':
            i = va_arg(args, uint32_t*);
            buf = dbr_unpack32(buf, i);
            break;
        case 'l':
            l = va_arg(args, uint64_t*);
            buf = dbr_unpack64(buf, l);
            break;
        case 'm':
            s = va_arg(args, char*);
            buf = dbr_unpack16(buf, &n);
            __builtin_memcpy(s, buf, dbr_min(n, MNEM_MAX));
            buf += n;
            break;
        case 's':
            j = va_arg(args, uint32_t);
            s = va_arg(args, char*);
            buf = dbr_unpack16(buf, &n);
            __builtin_memcpy(s, buf, dbr_min(n, j));
            buf += n;
            break;
        default:
            dbr_err_set(DBR_EINVAL, "invalid format character '%c'", *cp);
            return NULL;
        }
    }
    return buf;
}

DBR_API ssize_t
dbr_sizeof(const char* format, va_list args)
{
    ssize_t size = 0;
    for (const char* cp = format; *cp != '\0'; ++cp) {
        uint32_t i;
        const char* s;
        switch (*cp) {
        case 'd':
        case 'i':
            va_arg(args, uint32_t);
            size += sizeof(uint32_t);
            break;
        case 'l':
            va_arg(args, uint64_t);
            size += sizeof(uint64_t);
            break;
        case 'm':
            s = va_arg(args, const char*);
            i = strnlen(s, MNEM_MAX);
            size += sizeof(uint16_t) + i;
            break;
        case 's':
            i = va_arg(args, uint32_t);
            s = va_arg(args, const char*);
            i = strnlen(s, i);
            size += sizeof(uint16_t) + i;
            break;
        default:
            dbr_err_set(DBR_EINVAL, "invalid format character '%c'", *cp);
            return -1;
        }
    }
    return size;
}
