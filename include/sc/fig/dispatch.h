/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_FIG_DISPATCH_H
#define SC_FIG_DISPATCH_H

#include <sc/fig/clnt.h>
#include <sc/fig/handler.h>

/**
 * @addtogroup Dispatch
 * @{
 */

SC_API ScBool
sc_clnt_dispatch(ScClnt clnt, ScMillis ms, ScHandler handler);

/** @} */

#endif // SC_FIG_DISPATCH_H
