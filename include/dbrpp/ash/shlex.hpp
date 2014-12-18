/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ASH_SHLEX_HPP
#define DBRPP_ASH_SHLEX_HPP

#include <dbrpp/ash/except.hpp>

#include <dbr/ash/shlex.h>

namespace dbr {

/**
 * @addtogroup Shlex
 * @{
 */

template <typename FnT>
class Shlex {
    FnT fn_;
    DbrShlex impl_;
    static void
    cb(void* ctx, const char* tok, size_t len) noexcept
    {
        static_cast<Shlex*>(ctx)->fn_(tok, len);
    }
public:
    explicit
    Shlex(FnT fn) noexcept
        : fn_{fn}
    {
        dbr_shlex_init(&impl_, cb, this);
    }
    void
    reset() noexcept
    {
        dbr_shlex_reset(&impl_);
    }
    void
    exec(const char* buf, size_t size)
    {
        if (!dbr_shlex_exec(&impl_, buf, size))
            throw_exception();
    }
};

template <typename FnT>
Shlex<FnT>
make_shlex(FnT fn)
{
    return Shlex<FnT>{fn};
}

/** @} */

} // dbr

#endif // DBRPP_ASH_SHLEX_HPP
