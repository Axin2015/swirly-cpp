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
    DBR_CONN_EXEC = 1,
    DBR_CONN_MD
};

typedef struct DbrIHandler {
    const struct DbrHandlerVtbl* vtbl;
}* DbrHandler;

struct DbrHandlerVtbl {

    void
    (*up)(DbrHandler handler, int conn);

    void
    (*down)(DbrHandler handler, int conn);

    void
    (*timeout)(DbrHandler handler, DbrIden req_id);

    void
    (*status)(DbrHandler handler, DbrIden req_id, int num, const char* msg);

    void
    (*exec)(DbrHandler handler, DbrIden req_id, struct DbrExec* exec);

    void
    (*posn)(DbrHandler handler, struct DbrPosn* posn);

    void
    (*view)(DbrHandler handler, struct DbrView* view);

    void
    (*flush)(DbrHandler handler);
};

static inline void
dbr_handler_up(DbrHandler handler, int conn)
{
    handler->vtbl->up(handler, conn);
}

static inline void
dbr_handler_down(DbrHandler handler, int conn)
{
    handler->vtbl->down(handler, conn);
}

static inline void
dbr_handler_timeout(DbrHandler handler, DbrIden req_id)
{
    handler->vtbl->timeout(handler, req_id);
}

static inline void
dbr_handler_status(DbrHandler handler, DbrIden req_id, int num, const char* msg)
{
    handler->vtbl->status(handler, req_id, num, msg);
}

/**
 * The dbr_exec_incref() function can be used to extend the lifetime beyond the scope of the
 * callback.
 */

static inline void
dbr_handler_exec(DbrHandler handler, DbrIden req_id, struct DbrExec* exec)
{
    handler->vtbl->exec(handler, req_id, exec);
}

static inline void
dbr_handler_posn(DbrHandler handler, struct DbrPosn* posn)
{
    handler->vtbl->posn(handler, posn);
}

static inline void
dbr_handler_view(DbrHandler handler, struct DbrView* view)
{
    handler->vtbl->view(handler, view);
}

static inline void
dbr_handler_flush(DbrHandler handler)
{
    handler->vtbl->flush(handler);
}

/** @} */

#endif // DBR_HANDLER_H
