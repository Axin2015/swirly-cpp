/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ELM_ZMQJOURN_HPP
#define DBRPP_ELM_ZMQJOURN_HPP

#include <dbrpp/elm/journ.hpp>

#include <dbr/elm/zmqjourn.h>

namespace dbr {

/**
 * @addtogroup Journ
 * @{
 */

inline JournPtr
zmqjourn_create(void* zctx, size_t capacity, DbrJourn (*factory)(void*), void* arg)
{
    JournPtr ptr{dbr_zmqjourn_create(zctx, capacity, factory, arg)};
    if (!ptr)
        throw_exception();
    return ptr;
}

/** @} */

} // dbr

#endif // DBRPP_ELM_ZMQJOURN_HPP
