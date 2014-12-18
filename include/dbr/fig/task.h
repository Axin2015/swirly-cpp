/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_FIG_TASK_H
#define DBR_FIG_TASK_H

#include <dbr/fig/async.h>
#include <dbr/fig/handler.h>

/**
 * @addtogroup Task
 * @{
 */

DBR_API int
dbr_task_call(DbrAsync async, int (*fn)(DbrHandler, DbrClnt, void*), void* arg);

DBR_API void*
dbr_task_on_async(DbrHandler handler, DbrClnt clnt, void* val);

/** @} */

#endif // DBR_FIG_TASK_H
