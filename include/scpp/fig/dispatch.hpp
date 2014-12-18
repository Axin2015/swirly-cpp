/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_FIG_DISPATCH_HPP
#define SCPP_FIG_DISPATCH_HPP

#include <scpp/ash/except.hpp>

#include <sc/fig/dispatch.h>

namespace sc {

/**
 * @addtogroup Dispatch
 * @{
 */

inline void
dispatch(ScClnt clnt, ScMillis ms, ScHandler handler)
{
    if (!sc_clnt_dispatch(clnt, ms, handler))
        throw_exception();
}

/** @} */

} // sc

#endif // SCPP_FIG_DISPATCH_HPP
