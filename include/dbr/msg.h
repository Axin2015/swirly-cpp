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
    DBR_READ_ENTITY_REQ,
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
	int type;
	union {
        struct {
            int num;
            char msg[DBR_ERRMSG_MAX];
        } status_rep;
        struct {
            int type;
            // Set by dbr_msg_len();
            size_t count;
            struct DbrSlNode* first;
        } entity_rep;
        struct {
            // Set by dbr_msg_len();
            size_t posn_count;
            size_t trade_count;
            struct DbrOrder* new_order;
            struct DbrSlNode* first_posn;
            struct DbrSlNode* first_trade;
        } result_rep;
        struct {
            int type;
        } read_entity_req;
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
            // Set by dbr_msg_len();
            size_t count;
            struct DbrSlNode* first;
        } write_trans_req;
    };
};

DBR_API size_t
dbr_msg_len(struct DbrMsg* msg);

DBR_API char*
dbr_write_msg(char* buf, const struct DbrMsg* msg);

DBR_API const char*
dbr_read_msg(const char* buf, DbrPool pool, struct DbrMsg* msg);

DBR_EXTERN DbrBool
dbr_recv_msg(void* sock, DbrPool pool, struct DbrMsg* msg);

DBR_EXTERN DbrBool
dbr_send_msg(void* sock, struct DbrMsg* msg);

/** @} */

#endif // DBR_MSG_H
