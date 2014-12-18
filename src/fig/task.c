/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <dbr/fig/task.h>

struct Task {
    int (*fn)(DbrHandler, DbrClnt, void*);
    void* arg;
};

DBR_API int
dbr_task_call(DbrAsync async, int (*fn)(DbrHandler, DbrClnt, void*), void* arg)
{
    struct Task t = { .fn = fn, .arg = arg };
    dbr_async_send(async, &t);
    void* val;
    dbr_async_recv(async, &val);
    return (int)(long)val;
}

DBR_API void*
dbr_task_on_async(DbrHandler handler, DbrClnt clnt, void* val)
{
    struct Task* t = val;
    return (void*)(long)t->fn(handler, clnt, t->arg);
}
