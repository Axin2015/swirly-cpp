/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_FIR_SQLJOURN_HPP
#define SCPP_FIR_SQLJOURN_HPP

#include <scpp/elm/journ.hpp>

#include <sc/fir/sqljourn.h>

namespace sc {

/**
 * @addtogroup Journ
 * @{
 */

inline JournPtr
sqljourn_create(const char* path)
{
    JournPtr ptr{sc_sqljourn_create(path)};
    if (!ptr)
        throw_exception();
    return ptr;
}

/** @} */

} // sc

#endif // SCPP_FIR_SQLJOURN_HPP
