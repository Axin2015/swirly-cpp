/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_FIR_SQLMODEL_HPP
#define SCPP_FIR_SQLMODEL_HPP

#include <scpp/elm/model.hpp>

#include <sc/fir/sqlmodel.h>

namespace sc {

/**
 * @addtogroup Model
 * @{
 */

inline ModelPtr
sqlmodel_create(const char* path)
{
    ModelPtr ptr{sc_sqlmodel_create(path)};
    if (!ptr)
        throw_exception();
    return ptr;
}

/** @} */

} // sc

#endif // SCPP_FIR_SQLMODEL_HPP
