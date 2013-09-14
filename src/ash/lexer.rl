// -*- c -*-
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
#include <dbr/lexer.h>

#include <stddef.h> // NULL

%%{
    machine lexer;

	any_except_nl = any - '\n';

    action begin_tok {
        lexer->len = 0;
    }
    action add_char {
        if (lexer->len < DBR_TOK_MAX)
            lexer->tok[lexer->len++] = fc;
        else {
            cs = lexer_error;
            fbreak;
        }
    }
    action hold_nl {
        fhold;
    }
    action end_tok {
        lexer->tok[lexer->len] = '\0';
        lexer->cb(lexer->ctx, lexer->tok, lexer->len);
    }
    action end_line {
        lexer->cb(lexer->ctx, NULL, 0);
    }
    btok =
        # Starts with characters other than space, newline or quote.
        start: (
            [^\t\n "'\\] @add_char -> char
          | '\\' any_except_nl @add_char -> char
          | '\\' '\n' -> char
        ),
        char: (
            [^\t\n "'\\] @add_char -> char
          | '\\' any_except_nl @add_char -> char
          | '\\' '\n' -> char
          | [ \t] -> final
          # Hold newline for end-of-line processing.
          | '\n' @hold_nl -> final
        );
    dtok =
        start: (
            '"' ->char
        ),
        char: (
            [^"\\] @add_char -> char
          | '\\' any_except_nl @add_char -> char
          | '\\' '\n' -> char
          | '"' -> final
        );
    stok =
        start: (
            '\'' ->char
        ),
        char: (
            [^'\\] @add_char -> char
          | '\\' any_except_nl @add_char -> char
          | '\\' '\n' -> char
          | '\'' -> final
        );
    white = [ \t];
    tok = (btok | dtok | stok) >begin_tok %end_tok;
    line = white* (tok white*)* '\n' @end_line;
    main := line*;
}%%

%% write data nofinal;

DBR_API void
dbr_lexer_init(struct DbrLexer* lexer, void (*cb)(void*, const char*, size_t), void* ctx)
{
    lexer->cb = cb;
    lexer->ctx = ctx;

    int cs;
    %% write init;
    lexer->cs = cs;
}

DBR_API void
dbr_lexer_reset(struct DbrLexer* lexer)
{
    int cs;
    %% write init;
    lexer->cs = cs;
}

DBR_API int
dbr_lexer_exec(struct DbrLexer* lexer, const char* buf, size_t size)
{
	const char* p = buf;
	const char* pe = p + size;

    int cs = lexer->cs;
	%% write exec;
    lexer->cs = cs;

	return cs == lexer_error;
}
