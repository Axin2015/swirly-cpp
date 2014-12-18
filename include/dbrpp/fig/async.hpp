/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_FIG_ASYNC_HPP
#define DBRPP_FIG_ASYNC_HPP

#include <dbrpp/ash/except.hpp>

#include <dbr/fig/async.h>

namespace dbr {

/**
 * @addtogroup Async
 * @{
 */

class Async {
    friend class Ctx;
    DbrAsync impl_;
    Async(DbrAsync impl) noexcept
        : impl_{impl}
    {
    }
public:
    ~Async() noexcept
    {
        if (impl_)
            dbr_async_destroy(impl_);
    }
    constexpr
    Async(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    Async(void* zctx, const DbrUuid uuid)
        : impl_{dbr_async_create(zctx, uuid)}
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrAsync() const noexcept
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
            dbr_async_destroy(impl_);
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
        if (!dbr_async_send(impl_, val))
            throw_exception();
    }
    void*
    recv()
    {
        void* val;
        if (!dbr_async_recv(impl_, &val))
            throw_exception();
        return val;
    }
};

/** @} */

} // dbr

#endif // DBRPP_FIG_ASYNC_HPP
