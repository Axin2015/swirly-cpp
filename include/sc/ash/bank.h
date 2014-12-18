/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_BANK_H
#define SC_ASH_BANK_H

#include <sc/ash/defs.h>

#include <stddef.h> // size_t

/**
 * A bank of registers (not a financial institution.)
 *
 * @addtogroup Bank
 * @{
 */

struct ScBank {
    int fd;
    size_t len;
    long* arr;
};

SC_API void
sc_bank_term(struct ScBank* bank);

SC_API ScBool
sc_bank_init(struct ScBank* bank, const char* path, size_t len);

static inline long
sc_bank_load(const struct ScBank* bank, size_t reg)
{
    return __atomic_load_n(bank->arr + reg, __ATOMIC_SEQ_CST);
}

static inline void
sc_bank_store(struct ScBank* bank, size_t reg, long val)
{
    __atomic_store_n(bank->arr + reg, val, __ATOMIC_SEQ_CST);
}

static inline long
sc_bank_add_fetch(struct ScBank* bank, size_t reg, long val)
{
    return __atomic_add_fetch(bank->arr + reg, val, __ATOMIC_SEQ_CST);
}

/** @} */

#endif // SC_ASH_BANK_H
