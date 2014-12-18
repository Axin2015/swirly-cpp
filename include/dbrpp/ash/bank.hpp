/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ASH_BANK_HPP
#define DBRPP_ASH_BANK_HPP

#include <dbrpp/ash/except.hpp>

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
