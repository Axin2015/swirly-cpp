/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_ZMQCTX_HPP
#define SCPP_ELM_ZMQCTX_HPP

#include <scpp/ash/except.hpp>

#include <assert.h>

#include <zmq.h>

namespace sc {

/**
 * @addtogroup Zmq
 * @{
 */

class ZmqCtx {
    void* impl_;
public:
    ~ZmqCtx() noexcept
    {
        if (impl_) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
            {
                const int rc{zmq_ctx_destroy(impl_)};
                assert(rc == 0);
            }
#pragma GCC diagnostic pop
        }
    }
    constexpr
    ZmqCtx(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    ZmqCtx()
        : impl_{zmq_ctx_new()}
    {
        if (!impl_) {
            sc_err_setf(SC_EIO, "zmq_ctx_new() failed: %s", zmq_strerror(zmq_errno()));
            throw_exception();
        }
    }
    void*
    c_arg() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    ZmqCtx(const ZmqCtx&) = delete;

    ZmqCtx&
    operator =(const ZmqCtx&) = delete;

    // Move semantics.

    ZmqCtx(ZmqCtx&& rhs) noexcept
        : impl_{nullptr}
    {
        swap(rhs);
    }
    ZmqCtx&
    operator =(ZmqCtx&& rhs) noexcept
    {
        if (impl_) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
            {
                const int rc{zmq_ctx_destroy(impl_)};
                assert(rc == 0);
            }
#pragma GCC diagnostic pop
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(ZmqCtx& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
};

/** @} */

} // sc

#endif // SCPP_ELM_ZMQCTX_HPP
