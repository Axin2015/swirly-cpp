/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_FIR_SQLJOURN_H
#define SC_FIR_SQLJOURN_H

#include <sc/elm/journ.h>

/**
 * @addtogroup Journ
 * @{
 */

SC_API ScJourn
sc_sqljourn_create(const char* path);

/** @} */

#endif // SC_FIR_SQLJOURN_H
