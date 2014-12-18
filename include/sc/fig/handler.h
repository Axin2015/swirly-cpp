/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_FIG_HANDLER_H
#define SC_FIG_HANDLER_H

#include <sc/fig/clnt.h>

struct ScExec;
struct ScPosn;
struct ScView;

/**
 * @addtogroup Handler
 * @{
 */

typedef struct ScIHandler {
    const struct ScHandlerVtbl* vtbl;
}* ScHandler;

/**
 * Handler implementations should comprise non-blocking operations to avoid starving the main IO
 * dispatch loop.
 */

struct ScHandlerVtbl {

    void
    (*on_close)(ScHandler handler, ScClnt clnt);

    void
    (*on_ready)(ScHandler handler, ScClnt clnt);

    void
    (*on_logon)(ScHandler handler, ScClnt clnt, ScIden req_id, ScIden aid);

    void
    (*on_logoff)(ScHandler handler, ScClnt clnt, ScIden req_id, ScIden aid);

    void
    (*on_reset)(ScHandler handler, ScClnt clnt);

    void
    (*on_timeout)(ScHandler handler, ScClnt clnt, ScIden req_id);

    void
    (*on_status)(ScHandler handler, ScClnt clnt, ScIden req_id, int num, const char* msg);

    void
    (*on_exec)(ScHandler handler, ScClnt clnt, ScIden req_id, struct ScExec* exec);

    void
    (*on_posn)(ScHandler handler, ScClnt clnt, struct ScPosn* posn);

    void
    (*on_view)(ScHandler handler, ScClnt clnt, struct ScView* view);

    void
    (*on_flush)(ScHandler handler, ScClnt clnt);

    void*
    (*on_async)(ScHandler handler, ScClnt clnt, void* val);
};

static inline void
sc_handler_on_close(ScHandler handler, ScClnt clnt)
{
    handler->vtbl->on_close(handler, clnt);
}

static inline void
sc_handler_on_ready(ScHandler handler, ScClnt clnt)
{
    handler->vtbl->on_ready(handler, clnt);
}

static inline void
sc_handler_on_logon(ScHandler handler, ScClnt clnt, ScIden req_id, ScIden aid)
{
    handler->vtbl->on_logon(handler, clnt, req_id, aid);
}

static inline void
sc_handler_on_logoff(ScHandler handler, ScClnt clnt, ScIden req_id, ScIden aid)
{
    handler->vtbl->on_logoff(handler, clnt, req_id, aid);
}

static inline void
sc_handler_on_reset(ScHandler handler, ScClnt clnt)
{
    handler->vtbl->on_reset(handler, clnt);
}

static inline void
sc_handler_on_timeout(ScHandler handler, ScClnt clnt, ScIden req_id)
{
    handler->vtbl->on_timeout(handler, clnt, req_id);
}

static inline void
sc_handler_on_status(ScHandler handler, ScClnt clnt, ScIden req_id, int num, const char* msg)
{
    handler->vtbl->on_status(handler, clnt, req_id, num, msg);
}

/**
 * The sc_exec_incref() function can be used to extend the lifetime beyond the scope of the
 * callback.
 */

static inline void
sc_handler_on_exec(ScHandler handler, ScClnt clnt, ScIden req_id, struct ScExec* exec)
{
    handler->vtbl->on_exec(handler, clnt, req_id, exec);
}

static inline void
sc_handler_on_posn(ScHandler handler, ScClnt clnt, struct ScPosn* posn)
{
    handler->vtbl->on_posn(handler, clnt, posn);
}

static inline void
sc_handler_on_view(ScHandler handler, ScClnt clnt, struct ScView* view)
{
    handler->vtbl->on_view(handler, clnt, view);
}

static inline void
sc_handler_on_flush(ScHandler handler, ScClnt clnt)
{
    handler->vtbl->on_flush(handler, clnt);
}

static inline void*
sc_handler_on_async(ScHandler handler, ScClnt clnt, void* val)
{
    return handler->vtbl->on_async(handler, clnt, val);
}

/** @} */

#endif // SC_FIG_HANDLER_H
