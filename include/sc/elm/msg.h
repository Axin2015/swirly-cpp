/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_MSG_H
#define SC_ASH_MSG_H

#include <sc/elm/types.h>

#include <stddef.h> // size_t

/**
 * @addtogroup Msg
 * @{
 */

enum {
    SC_SESS_NOOP,
    SC_SESS_OPEN,
    SC_SESS_CLOSE,
    SC_SESS_LOGON,
    SC_SESS_LOGOFF,

    SC_STATUS_REP,
    SC_ACCNT_LIST_REP,
    SC_CONTR_LIST_REP,
    SC_TRADER_LIST_REP,
    SC_GIVEUP_LIST_REP,
    SC_ORDER_LIST_REP,
    SC_EXEC_LIST_REP,
    SC_POSN_LIST_REP,
    SC_VIEW_LIST_REP,
    SC_EXEC_REP,
    SC_POSN_REP,

    SC_PLACE_ORDER_REQ,
    SC_REVISE_ORDER_ID_REQ,
    SC_REVISE_ORDER_REF_REQ,
    SC_CANCEL_ORDER_ID_REQ,
    SC_CANCEL_ORDER_REF_REQ,
    SC_ACK_TRADE_REQ,

    SC_READ_ENTITY_REQ,
    SC_INSERT_EXEC_LIST_REQ,
    SC_INSERT_EXEC_REQ,
    SC_UPDATE_EXEC_REQ
};

enum {
    /**
     * No-op is used for heartbeating.
     */
    SC_SESS_HEARTBT = SC_SESS_NOOP
};

struct ScBody {
    ScIden req_id;
    int sid;
	int type;
	union {
        // Session.
        struct {
            int hbint;
        } sess_open;
        struct {
            ScIden aid;
        } sess_logon;
        struct {
            ScIden aid;
        } sess_logoff;
        // Reply.
        struct {
            int num;
            char msg[SC_ERRMSG_MAX];
        } status_rep;
        struct {
            // sc_shared_rec_entry()
            // sc_shared_perm_entry()
            // sc_shared_order_entry()
            // sc_shared_exec_entry()
            // sc_shared_posn_entry()
            struct ScSlNode* first;
            /**
             * @privatesection
             */
            // Set by sc_body_len();
            size_t count_;
        } entity_list_rep;
        struct {
            // sc_shared_view_entry()
            struct ScSlNode* first;
            /**
             * @privatesection
             */
            // Set by sc_body_len();
            size_t count_;
        } view_list_rep;
        struct {
            struct ScExec* exec;
        } exec_rep;
        struct {
            struct ScPosn* posn;
        } posn_rep;
        // Request.
        struct {
            int type;
        } sess_entity_req;
        struct {
            ScIden tid;
            ScIden gid;
            ScIden cid;
            ScJd settl_day;
            ScRef ref;
            int action;
            ScTicks ticks;
            ScLots lots;
            ScLots min_lots;
        } place_order_req;
        struct {
            ScIden tid;
            ScIden id;
            ScLots lots;
        } revise_order_id_req;
        struct {
            ScIden tid;
            ScRef ref;
            ScLots lots;
        } revise_order_ref_req;
        struct {
            ScIden tid;
            ScIden id;
        } cancel_order_id_req;
        struct {
            ScIden tid;
            ScRef ref;
        } cancel_order_ref_req;
        struct {
            ScIden tid;
            ScIden id;
        } ack_trade_req;
        struct {
            // Bit set of entity-types.
            int type;
        } read_entity_req;
        struct {
            struct ScSlNode* first;
            /**
             * @privatesection
             */
            // Set by sc_body_len();
            size_t count_;
        } insert_exec_list_req;
        struct {
            struct ScExec* exec;
        } insert_exec_req;
        struct {
            ScIden id;
            ScMillis modified;
        } update_exec_req;
    };
};

struct ScMsg {
    ScUuid uuid;
    struct ScBody body;
};

SC_API size_t
sc_body_len(struct ScBody* body, ScBool enriched);

SC_API char*
sc_write_body(char* buf, const struct ScBody* body, ScBool enriched);

SC_API const char*
sc_read_body(const char* buf, ScPool pool, struct ScBody* body);

SC_API ScBool
sc_send_body(void* sock, struct ScBody* body, ScBool enriched);

SC_API ScBool
sc_send_msg(void* sock, const ScUuid uuid, struct ScBody* body, ScBool enriched);

SC_API ScBool
sc_recv_body(void* sock, ScPool pool, struct ScBody* body);

SC_API ScBool
sc_recv_msg(void* sock, ScPool pool, struct ScMsg* msg);

/** @} */

#endif // SC_ASH_MSG_H
