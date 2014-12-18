/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_FIR_SQLJOURN_HPP
#define DBRPP_FIR_SQLJOURN_HPP

#include <dbrpp/elm/journ.hpp>

#include <dbr/fir/sqljourn.h>

namespace dbr {

/**
 * @addtogroup Journ
 * @{
 */

inline JournPtr
sqljourn_create(const char* path)
{
    JournPtr ptr{dbr_sqljourn_create(path)};
    if (!ptr)
        throw_exception();
    return ptr;
}

/** @} */

} // dbr

#endif // DBRPP_FIR_SQLJOURN_HPP
