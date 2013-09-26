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

#include <stddef.h> // size_t

/**
 * @addtogroup Msg
 * @{
 */

enum {
    DBR_READ_ENTITY_REQ = 1,
    DBR_READ_ENTITY_REP,
    DBR_WRITE_TRANS_REQ,
    DBR_WRITE_TRANS_REP
};

struct DbrMsg {
	int type;
	union {
        struct {
            int type;
        } read_entity_req;
        struct {
            int type;
            // Set by dbr_msg_len();
            size_t count;
            struct DbrSlNode* first;
        } read_entity_rep;
        struct {
            // Set by dbr_msg_len();
            size_t count;
            struct DbrSlNode* first;
        } write_trans_req;
        struct {
        } write_trans_rep;
    };
};

DBR_EXTERN DbrBool
dbr_recv_msg(void* sock, DbrPool pool, struct DbrMsg* msg);

DBR_EXTERN DbrBool
dbr_send_msg(void* sock, struct DbrMsg* msg);

/** @} */

#endif // DBR_MSG_H
