/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_FIG_DISPATCH_H
#define DBR_FIG_DISPATCH_H

#include <dbr/fig/clnt.h>
#include <dbr/fig/handler.h>

/**
 * @addtogroup Dispatch
 * @{
 */

DBR_API DbrBool
dbr_clnt_dispatch(DbrClnt clnt, DbrMillis ms, DbrHandler handler);

/** @} */

#endif // DBR_FIG_DISPATCH_H
