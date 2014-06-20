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
