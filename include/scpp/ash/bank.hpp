/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ASH_BANK_HPP
#define SCPP_ASH_BANK_HPP

#include <scpp/ash/except.hpp>

#include <sc/ash/bank.h>

namespace sc {

/**
 * @addtogroup Bank
 * @{
 */

class Bank {
    ScBank impl_;
public:
    ~Bank() noexcept
    {
        sc_bank_term(&impl_);
    }
    Bank(const char* path, size_t len)
    {
        if (!sc_bank_init(&impl_, path, len))
            throw_exception();
    }
    operator ScBank&() noexcept
    {
        return impl_;
    }
    ScBank*
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
        return sc_bank_load(&impl_, reg);
    }
    void
    store(size_t reg, long val) noexcept
    {
        sc_bank_store(&impl_, reg, val);
    }
    long
    add_fetch(size_t reg, long val) noexcept
    {
        return sc_bank_add_fetch(&impl_, reg, val);
    }
};

/** @} */

} // sc

#endif // SCPP_ASH_BANK_HPP
