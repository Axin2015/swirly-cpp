/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_FIG_CTX_HPP
#define SCPP_FIG_CTX_HPP

#include <scpp/fig/async.hpp>

#include <sc/fig/ctx.h>

#include <uuid/uuid.h>

#include <iostream>

namespace sc {

/**
 * @addtogroup Ctx
 * @{
 */

class Ctx {
    ScCtx impl_;
public:
    ~Ctx() noexcept
    {
        if (impl_)
            sc_ctx_destroy(impl_);
    }
    constexpr
    Ctx(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    Ctx(const char* mdaddr, const char* traddr, ScMillis tmout, size_t capacity,
        ScHandler handler)
        : impl_{sc_ctx_create(mdaddr, traddr, tmout, capacity, handler)}
    {
        if (!impl_)
            throw_exception();
    }
    operator ScCtx() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Ctx(const Ctx&) = delete;

    Ctx&
    operator =(const Ctx&) = delete;

    // Move semantics.

    Ctx(Ctx&& rhs) noexcept
        : impl_{nullptr}
    {
        swap(rhs);
    }
    Ctx&
    operator =(Ctx&& rhs) noexcept
    {
        if (impl_) {
            sc_ctx_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(Ctx& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    Async
    async()
    {
        ScAsync impl = sc_ctx_async(impl_);
        if (!impl)
            throw_exception();
        return Async(impl);
    }
    const unsigned char*
    uuid() const noexcept
    {
        return sc_ctx_uuid(impl_);
    }
};

inline std::ostream&
operator <<(std::ostream& os, const Ctx& ctx)
{
    char buf[SC_UUID_MAX + 1];
    uuid_unparse_lower(ctx.uuid(), buf);
    return os << "uuid=" << buf;
}

/** @} */

} // sc

#endif // SCPP_FIG_CTX_HPP
