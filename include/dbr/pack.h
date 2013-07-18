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
#ifndef DBR_PACK_H
#define DBR_PACK_H

#include <dbr/defs.h>

#include <stdarg.h>
#include <stdint.h>

#include <sys/types.h> // ssize_t

/**
 * @addtogroup Util
 * @{
 */

DBR_API char*
dbr_pack_int8(char* buf, int8_t i);

DBR_API char*
dbr_pack_uint8(char* buf, uint8_t i);

DBR_API char*
dbr_pack_int16(char* buf, int16_t i);

DBR_API char*
dbr_pack_uint16(char* buf, uint16_t i);

DBR_API char*
dbr_pack_int32(char* buf, int32_t i);

DBR_API char*
dbr_pack_uint32(char* buf, uint32_t i);

DBR_API char*
dbr_pack_int64(char* buf, int64_t i);

DBR_API char*
dbr_pack_uint64(char* buf, uint64_t i);

DBR_API char*
dbr_packf(char* buf, const char* format, ...);

DBR_API char*
dbr_vpackf(char* buf, const char* format, va_list args);

DBR_API const char*
dbr_unpack_int8(const char* buf, int8_t* i);

DBR_API const char*
dbr_unpack_uint8(const char* buf, uint8_t* i);

DBR_API const char*
dbr_unpack_int16(const char* buf, int16_t* i);

DBR_API const char*
dbr_unpack_uint16(const char* buf, uint16_t* i);

DBR_API const char*
dbr_unpack_int32(const char* buf, int32_t* i);

DBR_API const char*
dbr_unpack_uint32(const char* buf, uint32_t* i);

DBR_API const char*
dbr_unpack_int64(const char* buf, int64_t* i);

DBR_API const char*
dbr_unpack_uint64(const char* buf, uint64_t* i);

DBR_API const char*
dbr_unpackf(const char* buf, const char* format, ...);

DBR_API const char*
dbr_vunpackf(const char* buf, const char* format, va_list args);

/** @} */

#endif // DBR_PACK_H
