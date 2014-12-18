/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_FIG_ASYNC_HPP
#define SCPP_FIG_ASYNC_HPP

#include <scpp/ash/except.hpp>

#include <sc/fig/async.h>

namespace sc {

/**
 * @addtogroup Async
 * @{
 */

class Async {
    friend class Ctx;
    ScAsync impl_;
    Async(ScAsync impl) noexcept
        : impl_{impl}
    {
    }
public:
    ~Async() noexcept
    {
        if (impl_)
            sc_async_destroy(impl_);
    }
    constexpr
    Async(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    Async(void* zctx, const ScUuid uuid)
        : impl_{sc_async_create(zctx, uuid)}
    {
        if (!impl_)
            throw_exception();
    }
    operator ScAsync() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Async(const Async&) = delete;

    Async&
    operator =(const Async&) = delete;

    // Move semantics.

    Async(Async&& rhs) noexcept
        : impl_{nullptr}
    {
        swap(rhs);
    }
    Async&
    operator =(Async&& rhs) noexcept
    {
        if (impl_) {
            sc_async_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(Async& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    void
    send(void* val)
    {
        if (!sc_async_send(impl_, val))
            throw_exception();
    }
    void*
    recv()
    {
        void* val;
        if (!sc_async_recv(impl_, &val))
            throw_exception();
        return val;
    }
};

/** @} */

} // sc

#endif // SCPP_FIG_ASYNC_HPP
