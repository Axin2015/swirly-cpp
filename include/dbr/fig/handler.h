/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_FIG_HANDLER_H
#define DBR_FIG_HANDLER_H

#include <dbr/fig/clnt.h>

struct DbrExec;
struct DbrPosn;
struct DbrView;

/**
 * @addtogroup Handler
 * @{
 */

typedef struct DbrIHandler {
    const struct DbrHandlerVtbl* vtbl;
}* DbrHandler;

/**
 * Handler implementations should comprise non-blocking operations to avoid starving the main IO
 * dispatch loop.
 */

struct DbrHandlerVtbl {

    void
    (*on_close)(DbrHandler handler, DbrClnt clnt);

    void
    (*on_ready)(DbrHandler handler, DbrClnt clnt);

    void
    (*on_logon)(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrIden aid);

    void
    (*on_logoff)(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrIden aid);

    void
    (*on_reset)(DbrHandler handler, DbrClnt clnt);

    void
    (*on_timeout)(DbrHandler handler, DbrClnt clnt, DbrIden req_id);

    void
    (*on_status)(DbrHandler handler, DbrClnt clnt, DbrIden req_id, int num, const char* msg);

    void
    (*on_exec)(DbrHandler handler, DbrClnt clnt, DbrIden req_id, struct DbrExec* exec);

    void
    (*on_posn)(DbrHandler handler, DbrClnt clnt, struct DbrPosn* posn);

    void
    (*on_view)(DbrHandler handler, DbrClnt clnt, struct DbrView* view);

    void
    (*on_flush)(DbrHandler handler, DbrClnt clnt);

    void*
    (*on_async)(DbrHandler handler, DbrClnt clnt, void* val);
};

static inline void
dbr_handler_on_close(DbrHandler handler, DbrClnt clnt)
{
    handler->vtbl->on_close(handler, clnt);
}

static inline void
dbr_handler_on_ready(DbrHandler handler, DbrClnt clnt)
{
    handler->vtbl->on_ready(handler, clnt);
}

static inline void
dbr_handler_on_logon(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrIden aid)
{
    handler->vtbl->on_logon(handler, clnt, req_id, aid);
}

static inline void
dbr_handler_on_logoff(DbrHandler handler, DbrClnt clnt, DbrIden req_id, DbrIden aid)
{
    handler->vtbl->on_logoff(handler, clnt, req_id, aid);
}

static inline void
dbr_handler_on_reset(DbrHandler handler, DbrClnt clnt)
{
    handler->vtbl->on_reset(handler, clnt);
}

static inline void
dbr_handler_on_timeout(DbrHandler handler, DbrClnt clnt, DbrIden req_id)
{
    handler->vtbl->on_timeout(handler, clnt, req_id);
}

static inline void
dbr_handler_on_status(DbrHandler handler, DbrClnt clnt, DbrIden req_id, int num, const char* msg)
{
    handler->vtbl->on_status(handler, clnt, req_id, num, msg);
}

/**
 * The dbr_exec_incref() function can be used to extend the lifetime beyond the scope of the
 * callback.
 */

static inline void
dbr_handler_on_exec(DbrHandler handler, DbrClnt clnt, DbrIden req_id, struct DbrExec* exec)
{
    handler->vtbl->on_exec(handler, clnt, req_id, exec);
}

static inline void
dbr_handler_on_posn(DbrHandler handler, DbrClnt clnt, struct DbrPosn* posn)
{
    handler->vtbl->on_posn(handler, clnt, posn);
}

static inline void
dbr_handler_on_view(DbrHandler handler, DbrClnt clnt, struct DbrView* view)
{
    handler->vtbl->on_view(handler, clnt, view);
}

static inline void
dbr_handler_on_flush(DbrHandler handler, DbrClnt clnt)
{
    handler->vtbl->on_flush(handler, clnt);
}

static inline void*
dbr_handler_on_async(DbrHandler handler, DbrClnt clnt, void* val)
{
    return handler->vtbl->on_async(handler, clnt, val);
}

/** @} */

#endif // DBR_FIG_HANDLER_H
