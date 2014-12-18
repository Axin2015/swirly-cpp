/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_FIR_SQLJOURN_H
#define DBR_FIR_SQLJOURN_H

#include <dbr/elm/journ.h>

/**
 * @addtogroup Journ
 * @{
 */

DBR_API DbrJourn
dbr_sqljourn_create(const char* path);

/** @} */

#endif // DBR_FIR_SQLJOURN_H
