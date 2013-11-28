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

#include <dbr/err.h>
#include <dbr/pool.h>
#include <dbr/types.h>

#include <stddef.h> // size_t

/**
 * @addtogroup Msg
 * @{
 */

enum {
    DBR_SESS_HB = 1,
    DBR_STATUS_REP,
    DBR_ENTITY_REP,
    DBR_CYCLE_REP,
    DBR_ORDER_REP,
    DBR_LOGON_REQ,
    DBR_LOGOFF_REQ,
    DBR_READ_ENTITY_REQ,
    DBR_SESS_ENTITY_REQ,
    DBR_PLACE_ORDER_REQ,
    DBR_REVISE_ORDER_ID_REQ,
    DBR_REVISE_ORDER_REF_REQ,
    DBR_CANCEL_ORDER_ID_REQ,
    DBR_CANCEL_ORDER_REF_REQ,
    DBR_ARCHIVE_TRADE_REQ,
    DBR_WRITE_TRANS_REQ
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
            // dbr_entity_rec_entry()
            // dbr_entity_order_entry()
            // dbr_entity_memb_entry()
            // dbr_entity_exec_entry()
            // dbr_entity_posn_entry()
            struct DbrSlNode* first;
            /**
             * @privatesection
             */
            // Set by dbr_body_len();
            size_t count_;
        } entity_rep;
        struct {
            struct DbrOrder* new_order;
            // dbr_entity_exec_entry()
            struct DbrSlNode* first_exec;
            // dbr_entity_posn_entry()
            struct DbrSlNode* first_posn;
            /**
             * @privatesection
             */
            // Set by dbr_body_len();
            size_t exec_count_;
            size_t posn_count_;
        } cycle_rep;
        struct {
            struct DbrOrder* order;
        } order_rep;

        // Request.
        struct {
            // Bit set of entity-types.
            int type;
        } read_entity_req;
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
        } archive_trade_req;
        struct {
            struct DbrSlNode* first;
            /**
             * @privatesection
             */
            // Set by dbr_body_len();
            size_t count_;
        } write_trans_req;
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
