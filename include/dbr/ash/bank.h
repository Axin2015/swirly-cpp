/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_BANK_H
#define DBR_ASH_BANK_H

#include <dbr/ash/defs.h>

#include <stddef.h> // size_t

/**
 * A bank of registers (not a financial institution.)
 *
 * @addtogroup Bank
 * @{
 */

struct DbrBank {
    int fd;
    size_t len;
    long* arr;
};

DBR_API void
dbr_bank_term(struct DbrBank* bank);

DBR_API DbrBool
dbr_bank_init(struct DbrBank* bank, const char* path, size_t len);

static inline long
dbr_bank_load(const struct DbrBank* bank, size_t reg)
{
    return __atomic_load_n(bank->arr + reg, __ATOMIC_SEQ_CST);
}

static inline void
dbr_bank_store(struct DbrBank* bank, size_t reg, long val)
{
    __atomic_store_n(bank->arr + reg, val, __ATOMIC_SEQ_CST);
}

static inline long
dbr_bank_add_fetch(struct DbrBank* bank, size_t reg, long val)
{
    return __atomic_add_fetch(bank->arr + reg, val, __ATOMIC_SEQ_CST);
}

/** @} */

#endif // DBR_ASH_BANK_H
