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
    DBR_STATUS_REP = 1,
    DBR_ENTITY_REP,
    DBR_RESULT_REP,
    DBR_ORDER_REP,
    DBR_READ_ENTITY_REQ,
    DBR_SESS_ENTITY_REQ,
    DBR_PLACE_ORDER_REQ,
    DBR_REVISE_ORDER_ID_REQ,
    DBR_REVISE_ORDER_REF_REQ,
    DBR_CANCEL_ORDER_ID_REQ,
    DBR_CANCEL_ORDER_REF_REQ,
    DBR_ARCHIVE_ORDER_REQ,
    DBR_ARCHIVE_TRADE_REQ,
    DBR_WRITE_TRANS_REQ
};

struct DbrMsg {
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
            // dbr_rec_entry()
            // dbr_order_entry()
            // dbr_memb_entry()
            // dbr_trade_entry()
            // dbr_posn_entry()
            struct DbrSlNode* first;
            /**
             * @privatesection
             */
            // Set by dbr_msg_len();
            size_t count_;
        } entity_rep;
        struct {
            struct DbrOrder* new_order;
            // dbr_result_posn_entry()
            struct DbrSlNode* first_posn;
            // dbr_result_trade_entry()
            struct DbrSlNode* first_trade;
            /**
             * @privatesection
             */
            // Set by dbr_msg_len();
            size_t posn_count_;
            size_t trade_count_;
        } result_rep;
        struct {
            struct DbrOrder* order;
        } order_rep;
        // Request.
        struct {
            // Bit set of entity-types.
            int type;
        } read_entity_req;
        struct {
            DbrMnem trader;
            int type;
        } sess_entity_req;
        struct {
            DbrMnem trader;
            DbrMnem accnt;
            DbrMnem contr;
            DbrDate settl_date;
            DbrRef ref;
            int action;
            DbrTicks ticks;
            DbrLots lots;
            DbrLots min;
            DbrFlags flags;
        } place_order_req;
        struct {
            DbrMnem trader;
            DbrIden id;
            DbrLots lots;
        } revise_order_id_req;
        struct {
            DbrMnem trader;
            DbrRef ref;
            DbrLots lots;
        } revise_order_ref_req;
        struct {
            DbrMnem trader;
            DbrIden id;
        } cancel_order_id_req;
        struct {
            DbrMnem trader;
            DbrRef ref;
        } cancel_order_ref_req;
        struct {
            DbrMnem trader;
            DbrIden id;
        } archive_order_req;
        struct {
            DbrMnem trader;
            DbrIden id;
        } archive_trade_req;
        struct {
            struct DbrSlNode* first;
            /**
             * @privatesection
             */
            // Set by dbr_msg_len();
            size_t count_;
        } write_trans_req;
    };
};

DBR_API size_t
dbr_msg_len(struct DbrMsg* msg, DbrBool enriched);

DBR_API char*
dbr_write_msg(char* buf, const struct DbrMsg* msg, DbrBool enriched);

DBR_API const char*
dbr_read_msg(const char* buf, DbrPool pool, struct DbrMsg* msg);

DBR_API DbrBool
dbr_send_msg(void* sock, struct DbrMsg* msg, DbrBool enriched);

DBR_API DbrBool
dbr_recv_msg(void* sock, DbrPool pool, struct DbrMsg* msg);

/** @} */

#endif // DBR_MSG_H
