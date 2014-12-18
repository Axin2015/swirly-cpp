/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_ZMQJOURN_HPP
#define SCPP_ELM_ZMQJOURN_HPP

#include <scpp/elm/journ.hpp>

#include <sc/elm/zmqjourn.h>

namespace sc {

/**
 * @addtogroup Journ
 * @{
 */

inline JournPtr
zmqjourn_create(void* zctx, size_t capacity, ScJourn (*factory)(void*), void* arg)
{
    JournPtr ptr{sc_zmqjourn_create(zctx, capacity, factory, arg)};
    if (!ptr)
        throw_exception();
    return ptr;
}

/** @} */

} // sc

#endif // SCPP_ELM_ZMQJOURN_HPP
