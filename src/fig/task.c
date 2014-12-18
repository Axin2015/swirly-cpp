/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/fig/task.h>

struct Task {
    int (*fn)(ScHandler, ScClnt, void*);
    void* arg;
};

SC_API int
sc_task_call(ScAsync async, int (*fn)(ScHandler, ScClnt, void*), void* arg)
{
    struct Task t = { .fn = fn, .arg = arg };
    sc_async_send(async, &t);
    void* val;
    sc_async_recv(async, &val);
    return (int)(long)val;
}

SC_API void*
sc_task_on_async(ScHandler handler, ScClnt clnt, void* val)
{
    struct Task* t = val;
    return (void*)(long)t->fn(handler, clnt, t->arg);
}
