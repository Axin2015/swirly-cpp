/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIG_CLNT_H
#define FIG_CLNT_H

#include <sc/fig/clnt.h>

#include "cache.h"
#include "ordidx.h"

#include <sc/ash/prioq.h>

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
    struct ScSess sess;
    void* mdsock;
    void* trsock;
    void* asock;
    ScIden id;
    ScMillis tmout;
    ScPool pool;
    unsigned state;
    struct FigCache cache;
    struct FigOrdIdx ordidx;
    struct ScTree views;
    struct ScPrioq prioq;
    ScMillis mdlast;
    // Internal request-id for open or close.
    ScIden close_id;
    ScIden open_id;
    zmq_pollitem_t items[FIG_NSOCK];
};

#if SC_DEBUG_LEVEL >= 1
SC_EXTERN void
fig_clnt_log_state(unsigned state);
#else  // SC_DEBUG_LEVEL < 1
static inline void
fig_clnt_log_state(unsigned state) { }
#endif // SC_DEBUG_LEVEL < 1

SC_EXTERN void
fig_clnt_sess_reset(ScClnt clnt);

SC_EXTERN ScIden
fig_clnt_sess_close(ScClnt clnt, ScMillis now);

SC_EXTERN ScIden
fig_clnt_sess_open(ScClnt clnt, ScMillis now);

#endif // FIG_CLNT_H
