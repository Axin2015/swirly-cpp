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
#ifndef DBR_MSG_H
#define DBR_MSG_H

#include <dbr/types.h>

#include <stddef.h> // size_t

/**
 * @addtogroup Msg
 * @{
 */

enum {
    DBR_SESS_NOOP,
    DBR_SESS_OPEN,
    DBR_SESS_CLOSE,
    DBR_SESS_LOGON,
    DBR_SESS_LOGOFF,

    DBR_STATUS_REP,
    DBR_TRADER_LIST_REP,
    DBR_ACCNT_LIST_REP,
    DBR_CONTR_LIST_REP,
    DBR_ORDER_LIST_REP,
    DBR_EXEC_LIST_REP,
    DBR_MEMB_LIST_REP,
    DBR_POSN_LIST_REP,
    DBR_VIEW_LIST_REP,
    DBR_EXEC_REP,
    DBR_POSN_REP,

    DBR_PLACE_ORDER_REQ,
    DBR_REVISE_ORDER_ID_REQ,
    DBR_REVISE_ORDER_REF_REQ,
    DBR_CANCEL_ORDER_ID_REQ,
    DBR_CANCEL_ORDER_REF_REQ,
    DBR_ACK_TRADE_REQ,

    DBR_READ_ENTITY_REQ,
    DBR_INSERT_EXEC_LIST_REQ,
    DBR_INSERT_EXEC_REQ,
    DBR_UPDATE_EXEC_REQ
};

enum {
    // No-op is used for heartbeating.
    DBR_SESS_HEARTBT = DBR_SESS_NOOP
};

struct DbrBody {
    DbrIden req_id;
	int type;
	union {
        // Session.
        struct {
            int hbint;
        } sess_open;
        struct {
            DbrIden tid;
        } sess_logon;
        struct {
            DbrIden tid;
        } sess_logoff;
        // Reply.
        struct {
            int num;
            char msg[DBR_ERRMSG_MAX];
        } status_rep;
        struct {
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
        } entity_list_rep;
        struct {
            // dbr_shared_view_entry()
            struct DbrSlNode* first;
            /**
             * @privatesection
             */
            // Set by dbr_body_len();
            size_t count_;
        } view_list_rep;
        struct {
            struct DbrExec* exec;
        } exec_rep;
        struct {
            struct DbrPosn* posn;
        } posn_rep;
        // Request.
        struct {
            int type;
        } sess_entity_req;
        struct {
            DbrIden tid;
            DbrIden aid;
            DbrIden cid;
            DbrDate settl_date;
            DbrRef ref;
            int action;
            DbrTicks ticks;
            DbrLots lots;
            DbrLots min_lots;
        } place_order_req;
        struct {
            DbrIden tid;
            DbrIden id;
            DbrLots lots;
        } revise_order_id_req;
        struct {
            DbrIden tid;
            DbrRef ref;
            DbrLots lots;
        } revise_order_ref_req;
        struct {
            DbrIden tid;
            DbrIden id;
        } cancel_order_id_req;
        struct {
            DbrIden tid;
            DbrRef ref;
        } cancel_order_ref_req;
        struct {
            DbrIden tid;
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
        } insert_exec_list_req;
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
    DbrUuid uuid;
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
dbr_send_msg(void* sock, const DbrUuid uuid, struct DbrBody* body, DbrBool enriched);

DBR_API DbrBool
dbr_recv_body(void* sock, DbrPool pool, struct DbrBody* body);

DBR_API DbrBool
dbr_recv_msg(void* sock, DbrPool pool, struct DbrMsg* msg);

/** @} */

#endif // DBR_MSG_H
