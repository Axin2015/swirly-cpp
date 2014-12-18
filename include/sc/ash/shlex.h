/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_SHLEX_H
#define SC_ASH_SHLEX_H

/**
 * @file
 * Lexical tokeniser.
 */

#include <sc/ash/defs.h>

#include <sys/types.h>

/**
 * @addtogroup Shlex
 * @{
 */

enum {
    SC_TOK_MAX = 127,
};

struct ScShlex {
    void (*cb)(void* zctx, const char*, size_t);
    void* ctx;
    int cs;
    char tok[SC_TOK_MAX + 1];
    size_t len;
};

SC_API void
sc_shlex_init(struct ScShlex* shlex, void (*cb)(void*, const char*, size_t), void* zctx);

SC_API void
sc_shlex_reset(struct ScShlex* shlex);

SC_API ScBool
sc_shlex_exec(struct ScShlex* shlex, const char* buf, size_t size);

/** @} */

#endif // SC_ASH_SHLEX_H
