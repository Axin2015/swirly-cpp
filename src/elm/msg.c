/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "msg.h"
#include "pool.h"
#include "proto.h"

#include <sc/elm/proto.h>
#include <sc/elm/refcount.h>

#include <sc/ash/err.h>
#include <sc/ash/pack.h>
#include <sc/ash/queue.h>

#include <stdlib.h> // abort()
#include <string.h>

#include <zmq.h>

static const char STATUS_REP_FORMAT[] = "is";
static const char PLACE_ORDER_REQ_FORMAT[] = "lllisilll";
static const char REVISE_ORDER_ID_REQ_FORMAT[] = "lll";
static const char REVISE_ORDER_REF_REQ_FORMAT[] = "lsl";
static const char CANCEL_ORDER_ID_REQ_FORMAT[] = "ll";
static const char CANCEL_ORDER_REF_REQ_FORMAT[] = "ls";
static const char ACK_TRADE_REQ_FORMAT[] = "ll";
static const char UPDATE_EXEC_REQ_FORMAT[] = "li";

static void
free_view_list(struct ScSlNode* first, ScPool pool)
{
    struct ScSlNode* node = first;
    while (node) {
        struct ScView* view = sc_shared_view_entry(node);
        node = node->next;
        elm_pool_free_view(pool, view);
    }
}

static const char*
read_accnt(const char* buf, ScPool pool, struct ScQueue* queue)
{
    struct ScRec* rec = elm_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    sc_rec_init(rec);

    if (!(buf = elm_proto_read_accnt(buf, rec))) {
        elm_pool_free_rec(pool, rec);
        goto fail1;
    }
    sc_queue_insert_back(queue, &rec->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_contr(const char* buf, ScPool pool, struct ScQueue* queue)
{
    struct ScRec* rec = elm_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    sc_rec_init(rec);

    if (!(buf = elm_proto_read_contr(buf, rec))) {
        elm_pool_free_rec(pool, rec);
        goto fail1;
    }
    sc_queue_insert_back(queue, &rec->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_perm(const char* buf, ScPool pool, struct ScQueue* queue)
{
    struct ScPerm* perm = elm_pool_alloc_perm(pool);
    if (!perm)
        goto fail1;
    sc_perm_init(perm);

    if (!(buf = elm_proto_read_perm(buf, perm))) {
        elm_pool_free_perm(pool, perm);
        goto fail1;
    }
    sc_queue_insert_back(queue, &perm->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_order(const char* buf, ScPool pool, struct ScQueue* queue)
{
    struct ScOrder* order = elm_pool_alloc_order(pool);
    if (!order)
        goto fail1;
    sc_order_init(order);

    if (!(buf = elm_proto_read_order(buf, order))) {
        elm_pool_free_order(pool, order);
        goto fail1;
    }
    sc_queue_insert_back(queue, &order->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_exec(const char* buf, ScPool pool, struct ScQueue* queue)
{
    struct ScExec* exec = elm_pool_alloc_exec(pool);
    if (!exec)
        goto fail1;
    sc_exec_init(exec);

    if (!(buf = elm_proto_read_exec(buf, exec))) {
        sc_exec_decref(exec, pool);
        goto fail1;
    }
    // Transfer ownership to queue. (Reference count remains 1.)
    sc_queue_insert_back(queue, &exec->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_posn(const char* buf, ScPool pool, struct ScQueue* queue)
{
    struct ScPosn* posn = elm_pool_alloc_posn(pool);
    if (!posn)
        goto fail1;
    sc_posn_init(posn);

    if (!(buf = elm_proto_read_posn(buf, posn))) {
        elm_pool_free_posn(pool, posn);
        goto fail1;
    }
    sc_queue_insert_back(queue, &posn->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_view(const char* buf, ScPool pool, struct ScQueue* queue)
{
    struct ScView* view = elm_pool_alloc_view(pool);
    if (!view)
        goto fail1;
    sc_view_init(view);

    if (!(buf = elm_proto_read_view(buf, view))) {
        elm_pool_free_view(pool, view);
        goto fail1;
    }
    sc_queue_insert_back(queue, &view->shared_node_);
    return buf;
 fail1:
    return NULL;
}

SC_EXTERN size_t
elm_body_len(struct ScBody* body, ScBool enriched)
{
    size_t n = sc_pack_lenl(body->req_id);
    n += sc_pack_leni(body->sid);
    n += sc_pack_leni(body->type);
    switch (body->type) {
    case SC_SESS_NOOP:
        break;
    case SC_SESS_OPEN:
        n += sc_pack_leni(body->sess_open.hbint);
        break;
    case SC_SESS_CLOSE:
        break;
    case SC_SESS_LOGON:
        n += sc_pack_lenl(body->sess_logon.aid);
        break;
    case SC_SESS_LOGOFF:
        n += sc_pack_lenl(body->sess_logoff.aid);
        break;
    case SC_STATUS_REP:
        n += sc_pack_lenf(STATUS_REP_FORMAT,
                           body->status_rep.num,
                           SC_ERRMSG_MAX, body->status_rep.msg);
        break;
    case SC_ACCNT_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScRec* rec = sc_shared_rec_entry(node);
            n += elm_proto_accnt_len(rec);
            ++body->entity_list_rep.count_;
        }
        n += sc_pack_lenz(body->entity_list_rep.count_);
        break;
    case SC_CONTR_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScRec* rec = sc_shared_rec_entry(node);
            n += elm_proto_contr_len(rec);
            ++body->entity_list_rep.count_;
        }
        n += sc_pack_lenz(body->entity_list_rep.count_);
        break;
    case SC_TRADER_LIST_REP:
    case SC_GIVEUP_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScPerm* perm = sc_shared_perm_entry(node);
            n += elm_proto_perm_len(perm, enriched);
            ++body->entity_list_rep.count_;
        }
        n += sc_pack_lenz(body->entity_list_rep.count_);
        break;
    case SC_ORDER_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScOrder* order = sc_shared_order_entry(node);
            n += elm_proto_order_len(order, enriched);
            ++body->entity_list_rep.count_;
        }
        n += sc_pack_lenz(body->entity_list_rep.count_);
        break;
    case SC_EXEC_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScExec* exec = sc_shared_exec_entry(node);
            n += elm_proto_exec_len(exec, enriched);
            ++body->entity_list_rep.count_;
        }
        n += sc_pack_lenz(body->entity_list_rep.count_);
        break;
    case SC_POSN_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScPosn* posn = sc_shared_posn_entry(node);
            n += elm_proto_posn_len(posn, enriched);
            ++body->entity_list_rep.count_;
        }
        n += sc_pack_lenz(body->entity_list_rep.count_);
        break;
    case SC_VIEW_LIST_REP:
        body->view_list_rep.count_ = 0;
        for (struct ScSlNode* node = body->view_list_rep.first; node; node = node->next) {
            struct ScView* view = sc_shared_view_entry(node);
            n += elm_proto_view_len(view, enriched);
            ++body->view_list_rep.count_;
        }
        n += sc_pack_lenz(body->view_list_rep.count_);
        break;
    case SC_EXEC_REP:
        n += elm_proto_exec_len(body->exec_rep.exec, enriched);
        break;
    case SC_POSN_REP:
        n += elm_proto_posn_len(body->posn_rep.posn, enriched);
        break;
    case SC_PLACE_ORDER_REQ:
        n += sc_pack_lenf(PLACE_ORDER_REQ_FORMAT,
                           body->place_order_req.tid,
                           body->place_order_req.gid,
                           body->place_order_req.cid,
                           body->place_order_req.settl_day,
                           SC_REF_MAX, body->place_order_req.ref,
                           body->place_order_req.action,
                           body->place_order_req.ticks,
                           body->place_order_req.lots,
                           body->place_order_req.min_lots);
        break;
    case SC_REVISE_ORDER_ID_REQ:
        n += sc_pack_lenf(REVISE_ORDER_ID_REQ_FORMAT,
                           body->revise_order_id_req.tid,
                           body->revise_order_id_req.id,
                           body->revise_order_id_req.lots);
        break;
    case SC_REVISE_ORDER_REF_REQ:
        n += sc_pack_lenf(REVISE_ORDER_REF_REQ_FORMAT,
                           body->revise_order_ref_req.tid,
                           SC_REF_MAX, body->revise_order_ref_req.ref,
                           body->revise_order_ref_req.lots);
        break;
    case SC_CANCEL_ORDER_ID_REQ:
        n += sc_pack_lenf(CANCEL_ORDER_ID_REQ_FORMAT,
                           body->cancel_order_id_req.tid,
                           body->cancel_order_id_req.id);
        break;
    case SC_CANCEL_ORDER_REF_REQ:
        n += sc_pack_lenf(CANCEL_ORDER_REF_REQ_FORMAT,
                           body->cancel_order_ref_req.tid,
                           SC_REF_MAX, body->cancel_order_ref_req.ref);
        break;
    case SC_ACK_TRADE_REQ:
        n += sc_pack_lenf(ACK_TRADE_REQ_FORMAT,
                           body->ack_trade_req.tid,
                           body->ack_trade_req.id);
        break;
    case SC_READ_ENTITY_REQ:
        n += sc_pack_leni(body->read_entity_req.type);
        break;
    case SC_INSERT_EXEC_LIST_REQ:
        body->insert_exec_list_req.count_ = 0;
        for (struct ScSlNode* node = body->insert_exec_list_req.first; node; node = node->next) {
            struct ScExec* exec = sc_shared_exec_entry(node);
            n += elm_proto_exec_len(exec, enriched);
            ++body->insert_exec_list_req.count_;
        }
        n += sc_pack_lenz(body->insert_exec_list_req.count_);
        break;
    case SC_INSERT_EXEC_REQ:
        n += elm_proto_exec_len(body->insert_exec_req.exec, enriched);
        break;
    case SC_UPDATE_EXEC_REQ:
        n += sc_pack_lenf(UPDATE_EXEC_REQ_FORMAT,
                           body->update_exec_req.id,
                           body->update_exec_req.modified);
        break;
    default:
        abort();
    }
    return n;
}

SC_EXTERN char*
elm_write_body(char* buf, const struct ScBody* body, ScBool enriched)
{
    buf = sc_packl(buf, body->req_id);
    buf = sc_packi(buf, body->sid);
    buf = sc_packi(buf, body->type);
    switch (body->type) {
    case SC_SESS_NOOP:
        break;
    case SC_SESS_OPEN:
        buf = sc_packi(buf, body->sess_open.hbint);
        break;
    case SC_SESS_CLOSE:
        break;
    case SC_SESS_LOGON:
        buf = sc_packl(buf, body->sess_logon.aid);
        break;
    case SC_SESS_LOGOFF:
        buf = sc_packl(buf, body->sess_logoff.aid);
        break;
    case SC_STATUS_REP:
        buf = sc_packf(buf, STATUS_REP_FORMAT,
                        body->status_rep.num,
                        SC_ERRMSG_MAX, body->status_rep.msg);
        break;
    case SC_ACCNT_LIST_REP:
        buf = sc_packz(buf, body->entity_list_rep.count_);
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScRec* rec = sc_shared_rec_entry(node);
            buf = elm_proto_write_accnt(buf, rec);
        }
        break;
    case SC_CONTR_LIST_REP:
        buf = sc_packz(buf, body->entity_list_rep.count_);
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScRec* rec = sc_shared_rec_entry(node);
            buf = elm_proto_write_contr(buf, rec);
        }
        break;
    case SC_TRADER_LIST_REP:
    case SC_GIVEUP_LIST_REP:
        buf = sc_packz(buf, body->entity_list_rep.count_);
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScPerm* perm = sc_shared_perm_entry(node);
            buf = elm_proto_write_perm(buf, perm, enriched);
        }
        break;
    case SC_ORDER_LIST_REP:
        buf = sc_packz(buf, body->entity_list_rep.count_);
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScOrder* order = sc_shared_order_entry(node);
            buf = elm_proto_write_order(buf, order, enriched);
        }
        break;
    case SC_EXEC_LIST_REP:
        buf = sc_packz(buf, body->entity_list_rep.count_);
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScExec* exec = sc_shared_exec_entry(node);
            buf = elm_proto_write_exec(buf, exec, enriched);
        }
        break;
    case SC_POSN_LIST_REP:
        buf = sc_packz(buf, body->entity_list_rep.count_);
        for (struct ScSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct ScPosn* posn = sc_shared_posn_entry(node);
            buf = elm_proto_write_posn(buf, posn, enriched);
        }
        break;
    case SC_VIEW_LIST_REP:
        buf = sc_packz(buf, body->view_list_rep.count_);
        for (struct ScSlNode* node = body->view_list_rep.first; node; node = node->next) {
            struct ScView* view = sc_shared_view_entry(node);
            buf = elm_proto_write_view(buf, view, enriched);
        }
        break;
    case SC_EXEC_REP:
        buf = elm_proto_write_exec(buf, body->exec_rep.exec, enriched);
        break;
    case SC_POSN_REP:
        buf = elm_proto_write_posn(buf, body->posn_rep.posn, enriched);
        break;
    case SC_PLACE_ORDER_REQ:
        buf = sc_packf(buf, PLACE_ORDER_REQ_FORMAT,
                        body->place_order_req.tid,
                        body->place_order_req.gid,
                        body->place_order_req.cid,
                        body->place_order_req.settl_day,
                        SC_REF_MAX, body->place_order_req.ref,
                        body->place_order_req.action,
                        body->place_order_req.ticks,
                        body->place_order_req.lots,
                        body->place_order_req.min_lots);
        break;
    case SC_REVISE_ORDER_ID_REQ:
        buf = sc_packf(buf, REVISE_ORDER_ID_REQ_FORMAT,
                        body->revise_order_id_req.tid,
                        body->revise_order_id_req.id,
                        body->revise_order_id_req.lots);
        break;
    case SC_REVISE_ORDER_REF_REQ:
        buf = sc_packf(buf, REVISE_ORDER_REF_REQ_FORMAT,
                        body->revise_order_ref_req.tid,
                        SC_REF_MAX, body->revise_order_ref_req.ref,
                        body->revise_order_ref_req.lots);
        break;
    case SC_CANCEL_ORDER_ID_REQ:
        buf = sc_packf(buf, CANCEL_ORDER_ID_REQ_FORMAT,
                        body->cancel_order_id_req.tid,
                        body->cancel_order_id_req.id);
        break;
    case SC_CANCEL_ORDER_REF_REQ:
        buf = sc_packf(buf, CANCEL_ORDER_REF_REQ_FORMAT,
                        body->cancel_order_ref_req.tid,
                        SC_REF_MAX, body->cancel_order_ref_req.ref);
        break;
    case SC_ACK_TRADE_REQ:
        buf = sc_packf(buf, ACK_TRADE_REQ_FORMAT,
                        body->ack_trade_req.tid,
                        body->ack_trade_req.id);
        break;
    case SC_READ_ENTITY_REQ:
        buf = sc_packi(buf, body->read_entity_req.type);
        break;
    case SC_INSERT_EXEC_LIST_REQ:
        buf = sc_packz(buf, body->insert_exec_list_req.count_);
        for (struct ScSlNode* node = body->insert_exec_list_req.first; node; node = node->next) {
            struct ScExec* exec = sc_shared_exec_entry(node);
            buf = elm_proto_write_exec(buf, exec, enriched);
        }
        break;
    case SC_INSERT_EXEC_REQ:
        buf = elm_proto_write_exec(buf, body->insert_exec_req.exec, enriched);
        break;
    case SC_UPDATE_EXEC_REQ:
        buf = sc_packf(buf, UPDATE_EXEC_REQ_FORMAT,
                        body->update_exec_req.id,
                        body->update_exec_req.modified);
        break;
    }
    return buf;
}

SC_EXTERN const char*
elm_read_body(const char* buf, ScPool pool, struct ScBody* body)
{
    ScIden req_id;
    if (!(buf = sc_unpackl(buf, &req_id)))
        goto fail1;
    int sid;
    if (!(buf = sc_unpacki(buf, &sid)))
        goto fail1;
    int type;
    if (!(buf = sc_unpacki(buf, &type)))
        goto fail1;
    body->req_id = req_id;
    body->sid = sid;
    body->type = type;
    struct ScQueue q;
    switch (type) {
    case SC_SESS_NOOP:
        break;
    case SC_SESS_OPEN:
        if (!(buf = sc_unpacki(buf, &body->sess_open.hbint)))
            goto fail1;
        break;
    case SC_SESS_CLOSE:
        break;
    case SC_SESS_LOGON:
        if (!(buf = sc_unpackl(buf, &body->sess_logon.aid)))
            goto fail1;
        break;
    case SC_SESS_LOGOFF:
        if (!(buf = sc_unpackl(buf, &body->sess_logoff.aid)))
            goto fail1;
        break;
    case SC_STATUS_REP:
        buf = sc_unpackf(buf, STATUS_REP_FORMAT,
                          &body->status_rep.num,
                          SC_ERRMSG_MAX, body->status_rep.msg);
        break;
    case SC_ACCNT_LIST_REP:
        if (!(buf = sc_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        sc_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_accnt(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, SC_ENTITY_ACCNT, sc_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = sc_queue_first(&q);
        break;
    case SC_CONTR_LIST_REP:
        if (!(buf = sc_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        sc_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_contr(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, SC_ENTITY_CONTR, sc_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = sc_queue_first(&q);
        break;
    case SC_TRADER_LIST_REP:
    case SC_GIVEUP_LIST_REP:
        if (!(buf = sc_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        sc_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_perm(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, SC_ENTITY_PERM, sc_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = sc_queue_first(&q);
        break;
    case SC_ORDER_LIST_REP:
        if (!(buf = sc_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        sc_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_order(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, SC_ENTITY_ORDER, sc_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = sc_queue_first(&q);
        break;
    case SC_EXEC_LIST_REP:
        if (!(buf = sc_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        sc_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_exec(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, SC_ENTITY_EXEC, sc_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = sc_queue_first(&q);
        break;
    case SC_POSN_LIST_REP:
        if (!(buf = sc_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        sc_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_posn(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, SC_ENTITY_POSN, sc_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = sc_queue_first(&q);
        break;
    case SC_VIEW_LIST_REP:
        if (!(buf = sc_unpackz(buf, &body->view_list_rep.count_)))
            goto fail1;
        sc_queue_init(&q);
        for (size_t i = 0; i < body->view_list_rep.count_; ++i) {
            if (!(buf = read_view(buf, pool, &q))) {
                free_view_list(sc_queue_first(&q), pool);
                goto fail1;
            }
        }
        body->view_list_rep.first = sc_queue_first(&q);
        break;
    case SC_EXEC_REP:
        // Exec.
        body->exec_rep.exec = elm_pool_alloc_exec(pool);
        if (!body->exec_rep.exec)
            goto fail1;
        sc_exec_init(body->exec_rep.exec);

        if (!(buf = elm_proto_read_exec(buf, body->exec_rep.exec))) {
            sc_exec_decref(body->exec_rep.exec, pool);
            goto fail1;
        }
        break;
    case SC_POSN_REP:
        // Posn.
        body->posn_rep.posn = elm_pool_alloc_posn(pool);
        if (!body->posn_rep.posn)
            goto fail1;
        sc_posn_init(body->posn_rep.posn);

        if (!(buf = elm_proto_read_posn(buf, body->posn_rep.posn))) {
            elm_pool_free_posn(pool, body->posn_rep.posn);
            goto fail1;
        }
        break;
    case SC_PLACE_ORDER_REQ:
        if (!(buf = sc_unpackf(buf, PLACE_ORDER_REQ_FORMAT,
                                &body->place_order_req.tid,
                                &body->place_order_req.gid,
                                &body->place_order_req.cid,
                                &body->place_order_req.settl_day,
                                SC_REF_MAX, body->place_order_req.ref,
                                &body->place_order_req.action,
                                &body->place_order_req.ticks,
                                &body->place_order_req.lots,
                                &body->place_order_req.min_lots)))
            goto fail1;
        break;
    case SC_REVISE_ORDER_ID_REQ:
        if (!(buf = sc_unpackf(buf, REVISE_ORDER_ID_REQ_FORMAT,
                                &body->revise_order_id_req.tid,
                                &body->revise_order_id_req.id,
                                &body->revise_order_id_req.lots)))
            goto fail1;
        break;
    case SC_REVISE_ORDER_REF_REQ:
        if (!(buf = sc_unpackf(buf, REVISE_ORDER_REF_REQ_FORMAT,
                                &body->revise_order_ref_req.tid,
                                SC_REF_MAX, body->revise_order_ref_req.ref,
                                &body->revise_order_ref_req.lots)))
            goto fail1;
        break;
    case SC_CANCEL_ORDER_ID_REQ:
        if (!(buf = sc_unpackf(buf, CANCEL_ORDER_ID_REQ_FORMAT,
                                &body->cancel_order_id_req.tid,
                                &body->cancel_order_id_req.id)))
            goto fail1;
        break;
    case SC_CANCEL_ORDER_REF_REQ:
        if (!(buf = sc_unpackf(buf, CANCEL_ORDER_REF_REQ_FORMAT,
                                &body->cancel_order_ref_req.tid,
                                SC_REF_MAX, body->cancel_order_ref_req.ref)))
            goto fail1;
        break;
    case SC_ACK_TRADE_REQ:
        if (!(buf = sc_unpackf(buf, ACK_TRADE_REQ_FORMAT,
                                &body->ack_trade_req.tid,
                                &body->ack_trade_req.id)))
            goto fail1;
        break;
    case SC_READ_ENTITY_REQ:
        if (!(buf = sc_unpacki(buf, &body->read_entity_req.type)))
            goto fail1;
        break;
    case SC_INSERT_EXEC_LIST_REQ:
        if (!(buf = sc_unpackz(buf, &body->insert_exec_list_req.count_)))
            goto fail1;
        sc_queue_init(&q);
        for (size_t i = 0; i < body->insert_exec_list_req.count_; ++i) {
            if (!(buf = read_exec(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, SC_ENTITY_EXEC, sc_queue_first(&q));
                goto fail1;
            }
        }
        body->insert_exec_list_req.first = sc_queue_first(&q);
        break;
    case SC_INSERT_EXEC_REQ:
        body->insert_exec_req.exec = elm_pool_alloc_exec(pool);
        if (!body->insert_exec_req.exec)
            goto fail1;
        sc_exec_init(body->exec_rep.exec);

        if (!(buf = elm_proto_read_exec(buf, body->insert_exec_req.exec))) {
            sc_exec_decref(body->insert_exec_req.exec, pool);
            goto fail1;
        }
        break;
    case SC_UPDATE_EXEC_REQ:
        if (!(buf = sc_unpackf(buf, UPDATE_EXEC_REQ_FORMAT,
                                &body->update_exec_req.id,
                                &body->update_exec_req.modified)))
            goto fail1;
        break;
    default:
        sc_err_setf(SC_EIO, "invalid body-type '%d'", type);
        goto fail1;
    }
    return buf;
 fail1:
    return NULL;
}

SC_EXTERN ScBool
elm_send_body(void* sock, struct ScBody* body, ScBool enriched)
{
    const size_t len = sc_body_len(body, enriched);
    char buf[len];

    if (!elm_write_body(buf, body, enriched)) {
        sc_err_set(SC_EIO, "elm_write_body() failed");
        goto fail1;
    }
    if (zmq_send(sock, buf, len, 0) != len) {
        sc_err_setf(SC_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    return SC_TRUE;
 fail1:
    return SC_FALSE;
}

SC_EXTERN ScBool
elm_send_msg(void* sock, const ScUuid uuid, struct ScBody* body, ScBool enriched)
{
    if (zmq_send(sock, uuid, 16, ZMQ_SNDMORE) != 16) {
        sc_err_setf(SC_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    return sc_send_body(sock, body, enriched);
 fail1:
    return SC_FALSE;
}

SC_EXTERN ScBool
elm_recv_body(void* sock, ScPool pool, struct ScBody* body)
{
    zmq_msg_t zmsg;
    if (zmq_msg_init(&zmsg) < 0) {
        sc_err_setf(SC_EIO, "zmq_msg_init() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    if (zmq_msg_recv(&zmsg, sock, 0) < 0) {
        const int num = zmq_errno() == EINTR ? SC_EINTR : SC_EIO;
        sc_err_setf(num, "zmq_msg_recv() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    if (!elm_read_body(zmq_msg_data(&zmsg), pool, body)) {
        sc_err_set(SC_EIO, "elm_read_body() failed");
        goto fail2;
    }
    zmq_msg_close(&zmsg);
    return SC_TRUE;
 fail2:
    zmq_msg_close(&zmsg);
 fail1:
    return SC_FALSE;
}

SC_EXTERN ScBool
elm_recv_msg(void* sock, ScPool pool, struct ScMsg* msg)
{
    zmq_msg_t zmsg;
    if (zmq_msg_init(&zmsg) < 0) {
        sc_err_setf(SC_EIO, "zmq_msg_init() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    if (zmq_msg_recv(&zmsg, sock, 0) < 0) {
        const int num = zmq_errno() == EINTR ? SC_EINTR : SC_EIO;
        sc_err_setf(num, "zmq_msg_recv() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    assert(zmq_msg_size(&zmsg) == 16);
    __builtin_memcpy(msg->uuid, zmq_msg_data(&zmsg), 16);
    zmq_msg_close(&zmsg);
    return elm_recv_body(sock, pool, &msg->body);
 fail2:
    zmq_msg_close(&zmsg);
 fail1:
    return SC_FALSE;
}

SC_API size_t
sc_body_len(struct ScBody* body, ScBool enriched)
{
    return elm_body_len(body, enriched);
}

SC_API char*
sc_write_body(char* buf, const struct ScBody* body, ScBool enriched)
{
    return elm_write_body(buf, body, enriched);
}

SC_API const char*
sc_read_body(const char* buf, ScPool pool, struct ScBody* body)
{
    return elm_read_body(buf, pool, body);
}

SC_API ScBool
sc_send_body(void* sock, struct ScBody* body, ScBool enriched)
{
    return elm_send_body(sock, body, enriched);
}

SC_API ScBool
sc_send_msg(void* sock, const ScUuid uuid, struct ScBody* body, ScBool enriched)
{
    return elm_send_msg(sock, uuid, body, enriched);
}

SC_API ScBool
sc_recv_body(void* sock, ScPool pool, struct ScBody* body)
{
    return elm_recv_body(sock, pool, body);
}

SC_API ScBool
sc_recv_msg(void* sock, ScPool pool, struct ScMsg* msg)
{
    return elm_recv_msg(sock, pool, msg);
}
