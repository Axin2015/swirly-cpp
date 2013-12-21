/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
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
#ifndef DBR_MSG_H
#define DBR_MSG_H

#include <dbr/pool.h>
#include <dbr/types.h>

#include <stddef.h> // size_t

/**
 * @addtogroup Msg
 * @{
 */

enum {
    DBR_SESS_LOGON = 1,
    DBR_SESS_LOGOFF,
    DBR_SESS_HEARTBT,

    DBR_STATUS_REP,
    DBR_ENTITY_REP,
    DBR_EXEC_REP,
    DBR_POSN_REP,
    DBR_VIEW_REP,

    DBR_PLACE_ORDER_REQ,
    DBR_REVISE_ORDER_ID_REQ,
    DBR_REVISE_ORDER_REF_REQ,
    DBR_CANCEL_ORDER_ID_REQ,
    DBR_CANCEL_ORDER_REF_REQ,
    DBR_ACK_TRADE_REQ,

    DBR_READ_ENTITY_REQ,
    DBR_INSERT_EXECS_REQ,
    DBR_INSERT_EXEC_REQ,
    DBR_UPDATE_EXEC_REQ
};

struct DbrHead {
    DbrMnem trader;
};

struct DbrBody {
    DbrIden req_id;
	int type;
	union {
        // Reply.
        struct {
            int num;
            char msg[DBR_ERRMSG_MAX];
        } status_rep;
        struct {
            int type;
            // dbr_shared_rec_entry()
            // dbr_shared_order_entry()
            // dbr_shared_memb_entry()
            // dbr_shared_exec_entry()
            // dbr_shared_posn_entry()
            struct DbrSlNode* first;
            /**
             * @privatesection
             */
            // Set by dbr_body_len();
            size_t count_;
        } entity_rep;
        struct {
            struct DbrExec* exec;
        } exec_rep;
        struct {
            struct DbrPosn* posn;
        } posn_rep;
        struct {
            struct DbrSlNode* first;
            /**
             * @privatesection
             */
            // Set by dbr_body_len();
            size_t count_;
        } view_rep;
        // Request.
        struct {
            int type;
        } sess_entity_req;
        struct {
            DbrMnem accnt;
            DbrMnem contr;
            DbrDate settl_date;
            DbrRef ref;
            int action;
            DbrTicks ticks;
            DbrLots lots;
            DbrLots min_lots;
        } place_order_req;
        struct {
            DbrIden id;
            DbrLots lots;
        } revise_order_id_req;
        struct {
            DbrRef ref;
            DbrLots lots;
        } revise_order_ref_req;
        struct {
            DbrIden id;
        } cancel_order_id_req;
        struct {
            DbrRef ref;
        } cancel_order_ref_req;
        struct {
            DbrIden id;
        } ack_trade_req;
        struct {
            // Bit set of entity-types.
            int type;
        } read_entity_req;
        struct {
            struct DbrSlNode* first;
            /**
             * @privatesection
             */
            // Set by dbr_body_len();
            size_t count_;
        } insert_execs_req;
        struct {
            struct DbrExec* exec;
        } insert_exec_req;
        struct {
            DbrIden id;
            DbrMillis modified;
        } update_exec_req;
    };
};

struct DbrMsg {
    struct DbrHead head;
    struct DbrBody body;
};

DBR_API size_t
dbr_body_len(struct DbrBody* body, DbrBool enriched);

DBR_API char*
dbr_write_body(char* buf, const struct DbrBody* body, DbrBool enriched);

DBR_API const char*
dbr_read_body(const char* buf, DbrPool pool, struct DbrBody* body);

DBR_API DbrBool
dbr_send_body(void* sock, struct DbrBody* body, DbrBool enriched);

DBR_API DbrBool
dbr_send_msg(void* sock, const char* trader, struct DbrBody* body, DbrBool enriched);

DBR_API DbrBool
dbr_recv_body(void* sock, DbrPool pool, struct DbrBody* body);

DBR_API DbrBool
dbr_recv_msg(void* sock, DbrPool pool, struct DbrMsg* msg);

/** @} */

#endif // DBR_MSG_H
