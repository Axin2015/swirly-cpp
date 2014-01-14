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
#ifndef DBR_SESS_H
#define DBR_SESS_H

#include <dbr/defs.h>

struct DbrExec;

/**
 * @addtogroup Sess
 * @{
 */

enum DbrConn {
    DBR_CONN_EXEC = 0x01,
    DBR_CONN_MD   = 0x02
};

typedef struct DbrISess {
    const struct DbrSessVtbl* vtbl;
}* DbrSess;

struct DbrSessVtbl {

    void
    (*up_handler)(DbrSess sess, int conn);

    void
    (*down_handler)(DbrSess sess, int conn);

    void
    (*status_handler)(DbrSess sess, DbrIden req_id, int num, const char* msg);

    void
    (*exec_handler)(DbrSess sess, DbrIden req_id, struct DbrExec* exec);

    void
    (*timeout_handler)(DbrSess sess, DbrIden req_id);
};

static inline void
dbr_sess_up_handler(DbrSess sess, int conn)
{
    sess->vtbl->up_handler(sess, conn);
}

static inline void
dbr_sess_down_handler(DbrSess sess, int conn)
{
    sess->vtbl->down_handler(sess, conn);
}

static inline void
dbr_sess_status_handler(DbrSess sess, DbrIden req_id, int num, const char* msg)
{
    sess->vtbl->status_handler(sess, req_id, num, msg);
}

/**
 * The dbr_exec_incref() function can be used to extend the lifetime beyond the scope of the
 * callback.
 */

static inline void
dbr_sess_exec_handler(DbrSess sess, DbrIden req_id, struct DbrExec* exec)
{
    sess->vtbl->exec_handler(sess, req_id, exec);
}

static inline void
dbr_sess_timeout_handler(DbrSess sess, DbrIden req_id)
{
    sess->vtbl->timeout_handler(sess, req_id);
}

/** @} */

#endif // DBR_SESS_H
