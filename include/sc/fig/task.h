/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_FIG_TASK_H
#define SC_FIG_TASK_H

#include <sc/fig/async.h>
#include <sc/fig/handler.h>

/**
 * @addtogroup Task
 * @{
 */

SC_API int
sc_task_call(ScAsync async, int (*fn)(ScHandler, ScClnt, void*), void* arg);

SC_API void*
sc_task_on_async(ScHandler handler, ScClnt clnt, void* val);

/** @} */

#endif // SC_FIG_TASK_H
