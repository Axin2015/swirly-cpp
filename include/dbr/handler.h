/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#ifndef DBR_HANDLER_H
#define DBR_HANDLER_H

#include <dbr/defs.h>

struct DbrExec;
struct DbrPosn;
struct DbrView;

/**
 * @addtogroup Handler
 * @{
 */

enum DbrConn {
    /**
     * Trading connection.
     */
    DBR_CONN_TR = 1,
    /**
     * Market-data connection.
     */
    DBR_CONN_MD
};

typedef struct DbrIHandler {
    const struct DbrHandlerVtbl* vtbl;
}* DbrHandler;

struct DbrHandlerVtbl {

    void
    (*on_up)(DbrHandler handler, int conn);

    void
    (*on_down)(DbrHandler handler, int conn);

    void
    (*on_logon)(DbrHandler handler, DbrIden tid);

    void
    (*on_logoff)(DbrHandler handler, DbrIden tid);

    void
    (*on_timeout)(DbrHandler handler, DbrIden req_id);

    void
    (*on_status)(DbrHandler handler, DbrIden req_id, int num, const char* msg);

    void
    (*on_exec)(DbrHandler handler, DbrIden req_id, struct DbrExec* exec);

    void
    (*on_posn)(DbrHandler handler, struct DbrPosn* posn);

    void
    (*on_view)(DbrHandler handler, struct DbrView* view);

    void
    (*on_flush)(DbrHandler handler);
};

static inline void
dbr_handler_on_up(DbrHandler handler, int conn)
{
    handler->vtbl->on_up(handler, conn);
}

static inline void
dbr_handler_on_down(DbrHandler handler, int conn)
{
    handler->vtbl->on_down(handler, conn);
}

static inline void
dbr_handler_on_logon(DbrHandler handler, DbrIden tid)
{
    handler->vtbl->on_logon(handler, tid);
}

static inline void
dbr_handler_on_logoff(DbrHandler handler, DbrIden tid)
{
    handler->vtbl->on_logoff(handler, tid);
}

static inline void
dbr_handler_on_timeout(DbrHandler handler, DbrIden req_id)
{
    handler->vtbl->on_timeout(handler, req_id);
}

static inline void
dbr_handler_on_status(DbrHandler handler, DbrIden req_id, int num, const char* msg)
{
    handler->vtbl->on_status(handler, req_id, num, msg);
}

/**
 * The dbr_exec_incref() function can be used to extend the lifetime beyond the scope of the
 * callback.
 */

static inline void
dbr_handler_on_exec(DbrHandler handler, DbrIden req_id, struct DbrExec* exec)
{
    handler->vtbl->on_exec(handler, req_id, exec);
}

static inline void
dbr_handler_on_posn(DbrHandler handler, struct DbrPosn* posn)
{
    handler->vtbl->on_posn(handler, posn);
}

static inline void
dbr_handler_on_view(DbrHandler handler, struct DbrView* view)
{
    handler->vtbl->on_view(handler, view);
}

static inline void
dbr_handler_on_flush(DbrHandler handler)
{
    handler->vtbl->on_flush(handler);
}

/** @} */

#endif // DBR_HANDLER_H
