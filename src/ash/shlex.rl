// -*- c -*-
/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/ash/shlex.h>
#include <sc/ash/err.h>

#include <stddef.h> // NULL

%%{
    machine shlex;

	any_except_nl = any - '\n';

    action begin_tok {
        shlex->len = 0;
    }
    action add_char {
        if (shlex->len < SC_TOK_MAX)
            shlex->tok[shlex->len++] = fc;
        else {
            cs = shlex_error;
            fbreak;
        }
    }
    action hold_char {
        fhold;
    }
    action end_tok {
        shlex->tok[shlex->len] = '\0';
        shlex->cb(shlex->ctx, shlex->tok, shlex->len);
    }
    action end_line {
        shlex->cb(shlex->ctx, NULL, 0);
    }
    tok = (
        start: (
            [^\t\n "#'\\] @add_char -> bare
          | '\\' any_except_nl @add_char -> bare
          | '\\' '\n' -> start
          | '"' -> dquot
          | '\'' -> squot
        ),
        bare: (
            [^\t\n "#'\\] @add_char -> bare
          | '\\' any_except_nl @add_char -> bare
          | '\\' '\n' -> bare
          | [\t\n "#'] @hold_char -> final
        ),
        dquot: (
            [^"\\] @add_char -> dquot
          | '\\' any_except_nl @add_char -> dquot
          | '\\' '\n' -> dquot
          | '"' -> final
        ),
        squot: (
            [^'\\] @add_char -> squot
          | '\\' any_except_nl @add_char -> squot
          | '\\' '\n' -> squot
          | '\'' -> final
        )
    ) >begin_tok %end_tok;
    white = [ \t]*;
    comment = '#' . [^\n]*;
    line = white (tok white)* comment? '\n' @end_line;
    main := line*;
}%%

%% write data nofinal;

SC_API void
sc_shlex_init(struct ScShlex* shlex, void (*cb)(void*, const char*, size_t), void* ctx)
{
    shlex->cb = cb;
    shlex->ctx = ctx;

    int cs;
    %% write init;
    shlex->cs = cs;
}

SC_API void
sc_shlex_reset(struct ScShlex* shlex)
{
    int cs;
    %% write init;
    shlex->cs = cs;
}

SC_API ScBool
sc_shlex_exec(struct ScShlex* shlex, const char* buf, size_t size)
{
	const char* p = buf;
	const char* pe = p + size;

    int cs = shlex->cs;
	%% write exec;
    shlex->cs = cs;

    if (cs == shlex_error) {
        sc_err_set(SC_EINVAL, "lexical error");
        sc_shlex_reset(shlex);
        return SC_FALSE;
    }
	return SC_TRUE;
}
