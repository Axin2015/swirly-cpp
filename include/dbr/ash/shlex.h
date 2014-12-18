/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
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
