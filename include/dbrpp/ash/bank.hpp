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
#ifndef DBRPP_ASH_BANK_HPP
#define DBRPP_ASH_BANK_HPP

#include <dbrpp/except.hpp>

#include <dbr/ash/bank.h>

namespace dbr {

/**
 * @addtogroup Bank
 * @{
 */

class Bank {
    DbrBank impl_;
public:
    ~Bank() noexcept
    {
        dbr_bank_term(&impl_);
    }
    Bank(const char* path, size_t len)
    {
        if (!dbr_bank_init(&impl_, path, len))
            throw_exception();
    }
    operator DbrBank&() noexcept
    {
        return impl_;
    }
    DbrBank*
    c_arg() noexcept
    {
        return &impl_;
    }

    // Copy semantics.

    Bank(const Bank&) = delete;

    Bank&
    operator =(const Bank&) = delete;

    long
    load(size_t reg) const noexcept
    {
        return dbr_bank_load(&impl_, reg);
    }
    void
    store(size_t reg, long val) noexcept
    {
        dbr_bank_store(&impl_, reg, val);
    }
    long
    add_fetch(size_t reg, long val) noexcept
    {
        return dbr_bank_add_fetch(&impl_, reg, val);
    }
};

/** @} */

} // dbr

#endif // DBRPP_ASH_BANK_HPP
