/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_FIR_SQLMODEL_HPP
#define DBRPP_FIR_SQLMODEL_HPP

#include <dbrpp/elm/model.hpp>

#include <dbr/fir/sqlmodel.h>

namespace dbr {

/**
 * @addtogroup Model
 * @{
 */

inline ModelPtr
sqlmodel_create(const char* path)
{
    ModelPtr ptr{dbr_sqlmodel_create(path)};
    if (!ptr)
        throw_exception();
    return ptr;
}

/** @} */

} // dbr

#endif // DBRPP_FIR_SQLMODEL_HPP
