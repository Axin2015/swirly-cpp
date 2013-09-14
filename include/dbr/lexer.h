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
#ifndef DBR_LEXER_H
#define DBR_LEXER_H

#include <dbr/defs.h>

#include <sys/types.h>

/**
 * @addtogroup Lexer
 * @{
 */

enum {
    DBR_TOK_MAX = 127,
};

struct DbrLexer {
    void (*cb)(void* ctx, const char*, size_t);
    void* ctx;
    int cs;
    char tok[DBR_TOK_MAX + 1];
    size_t len;
};

DBR_API void
dbr_lexer_init(struct DbrLexer* lexer, void (*cb)(void*, const char*, size_t), void* ctx);

DBR_API void
dbr_lexer_reset(struct DbrLexer* lexer);

DBR_API int
dbr_lexer_exec(struct DbrLexer* lexer, const char* buf, size_t size);

/** @} */

#endif // DBR_LEXER_H
