/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_FIG_DISPATCH_HPP
#define DBRPP_FIG_DISPATCH_HPP

#include <dbrpp/ash/except.hpp>

#include <dbr/fig/dispatch.h>

namespace dbr {

/**
 * @addtogroup Dispatch
 * @{
 */

inline void
dispatch(DbrClnt clnt, DbrMillis ms, DbrHandler handler)
{
    if (!dbr_clnt_dispatch(clnt, ms, handler))
        throw_exception();
}

/** @} */

} // dbr

#endif // DBRPP_FIG_DISPATCH_HPP
