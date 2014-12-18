/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_ZMQJOURN_H
#define DBR_ASH_ZMQJOURN_H

#include <dbr/elm/journ.h>

/**
 * @addtogroup Journ
 * @{
 */

/**
 * The factory will be called from a separate thread.
 */

DBR_API DbrJourn
dbr_zmqjourn_create(void* zctx, size_t capacity, DbrJourn (*factory)(void*), void* arg);

/** @} */

#endif // DBR_ASH_ZMQJOURN_H
