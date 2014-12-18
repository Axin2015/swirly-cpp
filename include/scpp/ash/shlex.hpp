/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ASH_SHLEX_HPP
#define SCPP_ASH_SHLEX_HPP

#include <scpp/ash/except.hpp>

#include <sc/ash/shlex.h>

namespace sc {

/**
 * @addtogroup Shlex
 * @{
 */

template <typename FnT>
class Shlex {
    FnT fn_;
    ScShlex impl_;
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
        sc_shlex_init(&impl_, cb, this);
    }
    void
    reset() noexcept
    {
        sc_shlex_reset(&impl_);
    }
    void
    exec(const char* buf, size_t size)
    {
        if (!sc_shlex_exec(&impl_, buf, size))
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

} // sc

#endif // SCPP_ASH_SHLEX_HPP
