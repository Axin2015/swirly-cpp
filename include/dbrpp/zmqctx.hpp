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
#ifndef DBRPP_ZMQCTX_HPP
#define DBRPP_ZMQCTX_HPP

#include <dbrpp/except.hpp>

#include <assert.h>
#include <zmq.h>

namespace dbr {

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
            dbr_err_setf(DBR_EIO, "zmq_ctx_new() failed: %s", zmq_strerror(zmq_errno()));
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

} // dbr

#endif // DBRPP_ZMQCTX_HPP
