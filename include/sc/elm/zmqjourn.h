/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_ZMQJOURN_H
#define SC_ASH_ZMQJOURN_H

#include <sc/elm/journ.h>

/**
 * @addtogroup Journ
 * @{
 */

/**
 * The factory will be called from a separate thread.
 */

SC_API ScJourn
sc_zmqjourn_create(void* zctx, size_t capacity, ScJourn (*factory)(void*), void* arg);

/** @} */

#endif // SC_ASH_ZMQJOURN_H
