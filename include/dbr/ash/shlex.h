/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#ifndef DBR_ASH_SHLEX_H
#define DBR_ASH_SHLEX_H

/**
 * @file
 * Lexical tokeniser.
 */

#include <dbr/ash/defs.h>

#include <sys/types.h>

/**
 * @addtogroup Shlex
 * @{
 */

enum {
    DBR_TOK_MAX = 127,
};

struct DbrShlex {
    void (*cb)(void* zctx, const char*, size_t);
    void* ctx;
    int cs;
    char tok[DBR_TOK_MAX + 1];
    size_t len;
};

DBR_API void
dbr_shlex_init(struct DbrShlex* shlex, void (*cb)(void*, const char*, size_t), void* zctx);

DBR_API void
dbr_shlex_reset(struct DbrShlex* shlex);

DBR_API DbrBool
dbr_shlex_exec(struct DbrShlex* shlex, const char* buf, size_t size);

/** @} */

#endif // DBR_ASH_SHLEX_H
