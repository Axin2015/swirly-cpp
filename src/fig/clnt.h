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
#ifndef FIG_CLNT_H
#define FIG_CLNT_H

#include <dbr/fig/clnt.h>

#include "cache.h"
#include "ordidx.h"

#include <dbr/ash/prioq.h>

#include <zmq.h>

enum {
    FIG_MDSOCK,
    FIG_TRSOCK,
    FIG_ASOCK,
    FIG_NSOCK
};

// Other constants.

enum {
    // Non-negative timer ids are reserved for internal use.

    // The outbound heartbeat timer is scheduled once the logon response has been received along
    // with the heartbeat interval.
    FIG_HBTMR = -1,

    FIG_MDTMR = -2,
    FIG_MDINT = 2000,
    FIG_MDTMOUT = (FIG_MDINT * 3) / 2,

    // The transaction heartbeat timer is scheduled when the logon request is sent during
    // initialisation.
    FIG_TRTMR       = -3,
    FIG_TRINT       = 5000,
    FIG_TRTMOUT     = (FIG_TRINT * 3) / 2,

    // The init() function is called when the first view list is received on the md socket. The
    // init() function is responsible for sending a sesion-open to the server. The is expected to
    // respond by returning the session-open and reference data.

    FIG_READY       = 0x00,
    FIG_DELTA_WAIT  = 0x01,
    FIG_OPEN_WAIT   = 0x02,
    FIG_ACCNT_WAIT  = 0x04,
    FIG_CONTR_WAIT  = 0x08,
    FIG_SNAP_WAIT   = 0x10,
    FIG_CLOSE_WAIT  = 0x20,
    FIG_CLOSED      = 0x40,

    FIG_REC_WAIT    = FIG_ACCNT_WAIT | FIG_CONTR_WAIT,
    FIG_ALL_WAIT    = FIG_DELTA_WAIT | FIG_OPEN_WAIT | FIG_REC_WAIT | FIG_SNAP_WAIT | FIG_CLOSE_WAIT
};

struct FigClnt {
    struct DbrSess sess;
    void* mdsock;
    void* trsock;
    void* asock;
    DbrIden id;
    DbrMillis tmout;
    DbrPool pool;
    unsigned state;
    struct FigCache cache;
    struct FigOrdIdx ordidx;
    struct DbrTree views;
    struct DbrPrioq prioq;
    DbrMillis mdlast;
    // Internal request-id for open or close.
    DbrIden close_id;
    DbrIden open_id;
    zmq_pollitem_t items[FIG_NSOCK];
};

#if DBR_DEBUG_LEVEL >= 1
DBR_EXTERN void
fig_clnt_log_state(unsigned state);
#else  // DBR_DEBUG_LEVEL < 1
static inline void
fig_clnt_log_state(unsigned state) { }
#endif // DBR_DEBUG_LEVEL < 1

DBR_EXTERN void
fig_clnt_sess_reset(DbrClnt clnt);

DBR_EXTERN DbrIden
fig_clnt_sess_close(DbrClnt clnt, DbrMillis now);

DBR_EXTERN DbrIden
fig_clnt_sess_open(DbrClnt clnt, DbrMillis now);

#endif // FIG_CLNT_H
