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
#include "msg.h"
#include "pool.h"
#include "proto.h"

#include <dbr/ash/err.h>
#include <dbr/ash/pack.h>
#include <dbr/elm/proto.h>
#include <dbr/ash/queue.h>
#include <dbr/elm/refcount.h>

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
free_view_list(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrView* view = dbr_shared_view_entry(node);
        node = node->next;
        elm_pool_free_view(pool, view);
    }
}

static const char*
read_accnt(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrRec* rec = elm_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    dbr_rec_init(rec);

    if (!(buf = elm_proto_read_accnt(buf, rec))) {
        elm_pool_free_rec(pool, rec);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &rec->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_contr(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrRec* rec = elm_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    dbr_rec_init(rec);

    if (!(buf = elm_proto_read_contr(buf, rec))) {
        elm_pool_free_rec(pool, rec);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &rec->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_memb(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrMemb* memb = elm_pool_alloc_memb(pool);
    if (!memb)
        goto fail1;
    dbr_memb_init(memb);

    if (!(buf = elm_proto_read_memb(buf, memb))) {
        elm_pool_free_memb(pool, memb);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &memb->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_order(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrOrder* order = elm_pool_alloc_order(pool);
    if (!order)
        goto fail1;
    dbr_order_init(order);

    if (!(buf = elm_proto_read_order(buf, order))) {
        elm_pool_free_order(pool, order);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &order->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_exec(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrExec* exec = elm_pool_alloc_exec(pool);
    if (!exec)
        goto fail1;
    dbr_exec_init(exec);

    if (!(buf = elm_proto_read_exec(buf, exec))) {
        dbr_exec_decref(exec, pool);
        goto fail1;
    }
    // Transfer ownership to queue. (Reference count remains 1.)
    dbr_queue_insert_back(queue, &exec->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_posn(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrPosn* posn = elm_pool_alloc_posn(pool);
    if (!posn)
        goto fail1;
    dbr_posn_init(posn);

    if (!(buf = elm_proto_read_posn(buf, posn))) {
        elm_pool_free_posn(pool, posn);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &posn->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_view(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrView* view = elm_pool_alloc_view(pool);
    if (!view)
        goto fail1;
    dbr_view_init(view);

    if (!(buf = elm_proto_read_view(buf, view))) {
        elm_pool_free_view(pool, view);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &view->shared_node_);
    return buf;
 fail1:
    return NULL;
}

DBR_EXTERN size_t
elm_body_len(struct DbrBody* body, DbrBool enriched)
{
    size_t n = dbr_pack_lenl(body->req_id);
    n += dbr_pack_leni(body->type);
    switch (body->type) {
    case DBR_SESS_NOOP:
        break;
    case DBR_SESS_OPEN:
        n += dbr_pack_leni(body->sess_open.hbint);
        break;
    case DBR_SESS_CLOSE:
        break;
    case DBR_SESS_LOGON:
        n += dbr_pack_lenl(body->sess_logon.uid);
        break;
    case DBR_SESS_LOGOFF:
        n += dbr_pack_lenl(body->sess_logoff.uid);
        break;
    case DBR_STATUS_REP:
        n += dbr_pack_lenf(STATUS_REP_FORMAT,
                           body->status_rep.num,
                           DBR_ERRMSG_MAX, body->status_rep.msg);
        break;
    case DBR_ACCNT_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrRec* rec = dbr_shared_rec_entry(node);
            n += elm_proto_accnt_len(rec);
            ++body->entity_list_rep.count_;
        }
        n += dbr_pack_lenz(body->entity_list_rep.count_);
        break;
    case DBR_CONTR_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrRec* rec = dbr_shared_rec_entry(node);
            n += elm_proto_contr_len(rec);
            ++body->entity_list_rep.count_;
        }
        n += dbr_pack_lenz(body->entity_list_rep.count_);
        break;
    case DBR_USER_LIST_REP:
    case DBR_GROUP_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrMemb* memb = dbr_shared_memb_entry(node);
            n += elm_proto_memb_len(memb, enriched);
            ++body->entity_list_rep.count_;
        }
        n += dbr_pack_lenz(body->entity_list_rep.count_);
        break;
    case DBR_ORDER_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrOrder* order = dbr_shared_order_entry(node);
            n += elm_proto_order_len(order, enriched);
            ++body->entity_list_rep.count_;
        }
        n += dbr_pack_lenz(body->entity_list_rep.count_);
        break;
    case DBR_EXEC_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrExec* exec = dbr_shared_exec_entry(node);
            n += elm_proto_exec_len(exec, enriched);
            ++body->entity_list_rep.count_;
        }
        n += dbr_pack_lenz(body->entity_list_rep.count_);
        break;
    case DBR_POSN_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrPosn* posn = dbr_shared_posn_entry(node);
            n += elm_proto_posn_len(posn, enriched);
            ++body->entity_list_rep.count_;
        }
        n += dbr_pack_lenz(body->entity_list_rep.count_);
        break;
    case DBR_VIEW_LIST_REP:
        body->view_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->view_list_rep.first; node; node = node->next) {
            struct DbrView* view = dbr_shared_view_entry(node);
            n += elm_proto_view_len(view, enriched);
            ++body->view_list_rep.count_;
        }
        n += dbr_pack_lenz(body->view_list_rep.count_);
        break;
    case DBR_EXEC_REP:
        n += elm_proto_exec_len(body->exec_rep.exec, enriched);
        break;
    case DBR_POSN_REP:
        n += elm_proto_posn_len(body->posn_rep.posn, enriched);
        break;
    case DBR_PLACE_ORDER_REQ:
        n += dbr_pack_lenf(PLACE_ORDER_REQ_FORMAT,
                           body->place_order_req.uid,
                           body->place_order_req.gid,
                           body->place_order_req.cid,
                           body->place_order_req.settl_day,
                           DBR_REF_MAX, body->place_order_req.ref,
                           body->place_order_req.action,
                           body->place_order_req.ticks,
                           body->place_order_req.lots,
                           body->place_order_req.min_lots);
        break;
    case DBR_REVISE_ORDER_ID_REQ:
        n += dbr_pack_lenf(REVISE_ORDER_ID_REQ_FORMAT,
                           body->revise_order_id_req.uid,
                           body->revise_order_id_req.id,
                           body->revise_order_id_req.lots);
        break;
    case DBR_REVISE_ORDER_REF_REQ:
        n += dbr_pack_lenf(REVISE_ORDER_REF_REQ_FORMAT,
                           body->revise_order_ref_req.uid,
                           DBR_REF_MAX, body->revise_order_ref_req.ref,
                           body->revise_order_ref_req.lots);
        break;
    case DBR_CANCEL_ORDER_ID_REQ:
        n += dbr_pack_lenf(CANCEL_ORDER_ID_REQ_FORMAT,
                           body->cancel_order_id_req.uid,
                           body->cancel_order_id_req.id);
        break;
    case DBR_CANCEL_ORDER_REF_REQ:
        n += dbr_pack_lenf(CANCEL_ORDER_REF_REQ_FORMAT,
                           body->cancel_order_ref_req.uid,
                           DBR_REF_MAX, body->cancel_order_ref_req.ref);
        break;
    case DBR_ACK_TRADE_REQ:
        n += dbr_pack_lenf(ACK_TRADE_REQ_FORMAT,
                           body->ack_trade_req.uid,
                           body->ack_trade_req.id);
        break;
    case DBR_READ_ENTITY_REQ:
        n += dbr_pack_leni(body->read_entity_req.type);
        break;
    case DBR_INSERT_EXEC_LIST_REQ:
        body->insert_exec_list_req.count_ = 0;
        for (struct DbrSlNode* node = body->insert_exec_list_req.first; node; node = node->next) {
            struct DbrExec* exec = dbr_shared_exec_entry(node);
            n += elm_proto_exec_len(exec, enriched);
            ++body->insert_exec_list_req.count_;
        }
        n += dbr_pack_lenz(body->insert_exec_list_req.count_);
        break;
    case DBR_INSERT_EXEC_REQ:
        n += elm_proto_exec_len(body->insert_exec_req.exec, enriched);
        break;
    case DBR_UPDATE_EXEC_REQ:
        n += dbr_pack_lenf(UPDATE_EXEC_REQ_FORMAT,
                           body->update_exec_req.id,
                           body->update_exec_req.modified);
        break;
    default:
        abort();
    }
    return n;
}

DBR_EXTERN char*
elm_write_body(char* buf, const struct DbrBody* body, DbrBool enriched)
{
    buf = dbr_packl(buf, body->req_id);
    buf = dbr_packi(buf, body->type);
    switch (body->type) {
    case DBR_SESS_NOOP:
        break;
    case DBR_SESS_OPEN:
        buf = dbr_packi(buf, body->sess_open.hbint);
        break;
    case DBR_SESS_CLOSE:
        break;
    case DBR_SESS_LOGON:
        buf = dbr_packl(buf, body->sess_logon.uid);
        break;
    case DBR_SESS_LOGOFF:
        buf = dbr_packl(buf, body->sess_logoff.uid);
        break;
    case DBR_STATUS_REP:
        buf = dbr_packf(buf, STATUS_REP_FORMAT,
                        body->status_rep.num,
                        DBR_ERRMSG_MAX, body->status_rep.msg);
        break;
    case DBR_ACCNT_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrRec* rec = dbr_shared_rec_entry(node);
            buf = elm_proto_write_accnt(buf, rec);
        }
        break;
    case DBR_CONTR_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrRec* rec = dbr_shared_rec_entry(node);
            buf = elm_proto_write_contr(buf, rec);
        }
        break;
    case DBR_USER_LIST_REP:
    case DBR_GROUP_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrMemb* memb = dbr_shared_memb_entry(node);
            buf = elm_proto_write_memb(buf, memb, enriched);
        }
        break;
    case DBR_ORDER_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrOrder* order = dbr_shared_order_entry(node);
            buf = elm_proto_write_order(buf, order, enriched);
        }
        break;
    case DBR_EXEC_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrExec* exec = dbr_shared_exec_entry(node);
            buf = elm_proto_write_exec(buf, exec, enriched);
        }
        break;
    case DBR_POSN_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrPosn* posn = dbr_shared_posn_entry(node);
            buf = elm_proto_write_posn(buf, posn, enriched);
        }
        break;
    case DBR_VIEW_LIST_REP:
        buf = dbr_packz(buf, body->view_list_rep.count_);
        for (struct DbrSlNode* node = body->view_list_rep.first; node; node = node->next) {
            struct DbrView* view = dbr_shared_view_entry(node);
            buf = elm_proto_write_view(buf, view, enriched);
        }
        break;
    case DBR_EXEC_REP:
        buf = elm_proto_write_exec(buf, body->exec_rep.exec, enriched);
        break;
    case DBR_POSN_REP:
        buf = elm_proto_write_posn(buf, body->posn_rep.posn, enriched);
        break;
    case DBR_PLACE_ORDER_REQ:
        buf = dbr_packf(buf, PLACE_ORDER_REQ_FORMAT,
                        body->place_order_req.uid,
                        body->place_order_req.gid,
                        body->place_order_req.cid,
                        body->place_order_req.settl_day,
                        DBR_REF_MAX, body->place_order_req.ref,
                        body->place_order_req.action,
                        body->place_order_req.ticks,
                        body->place_order_req.lots,
                        body->place_order_req.min_lots);
        break;
    case DBR_REVISE_ORDER_ID_REQ:
        buf = dbr_packf(buf, REVISE_ORDER_ID_REQ_FORMAT,
                        body->revise_order_id_req.uid,
                        body->revise_order_id_req.id,
                        body->revise_order_id_req.lots);
        break;
    case DBR_REVISE_ORDER_REF_REQ:
        buf = dbr_packf(buf, REVISE_ORDER_REF_REQ_FORMAT,
                        body->revise_order_ref_req.uid,
                        DBR_REF_MAX, body->revise_order_ref_req.ref,
                        body->revise_order_ref_req.lots);
        break;
    case DBR_CANCEL_ORDER_ID_REQ:
        buf = dbr_packf(buf, CANCEL_ORDER_ID_REQ_FORMAT,
                        body->cancel_order_id_req.uid,
                        body->cancel_order_id_req.id);
        break;
    case DBR_CANCEL_ORDER_REF_REQ:
        buf = dbr_packf(buf, CANCEL_ORDER_REF_REQ_FORMAT,
                        body->cancel_order_ref_req.uid,
                        DBR_REF_MAX, body->cancel_order_ref_req.ref);
        break;
    case DBR_ACK_TRADE_REQ:
        buf = dbr_packf(buf, ACK_TRADE_REQ_FORMAT,
                        body->ack_trade_req.uid,
                        body->ack_trade_req.id);
        break;
    case DBR_READ_ENTITY_REQ:
        buf = dbr_packi(buf, body->read_entity_req.type);
        break;
    case DBR_INSERT_EXEC_LIST_REQ:
        buf = dbr_packz(buf, body->insert_exec_list_req.count_);
        for (struct DbrSlNode* node = body->insert_exec_list_req.first; node; node = node->next) {
            struct DbrExec* exec = dbr_shared_exec_entry(node);
            buf = elm_proto_write_exec(buf, exec, enriched);
        }
        break;
    case DBR_INSERT_EXEC_REQ:
        buf = elm_proto_write_exec(buf, body->insert_exec_req.exec, enriched);
        break;
    case DBR_UPDATE_EXEC_REQ:
        buf = dbr_packf(buf, UPDATE_EXEC_REQ_FORMAT,
                        body->update_exec_req.id,
                        body->update_exec_req.modified);
        break;
    }
    return buf;
}

DBR_EXTERN const char*
elm_read_body(const char* buf, DbrPool pool, struct DbrBody* body)
{
    DbrIden req_id;
    if (!(buf = dbr_unpackl(buf, &req_id)))
        goto fail1;
    int type;
    if (!(buf = dbr_unpacki(buf, &type)))
        goto fail1;
    body->req_id = req_id;
    body->type = type;
    struct DbrQueue q;
    switch (type) {
    case DBR_SESS_NOOP:
        break;
    case DBR_SESS_OPEN:
        if (!(buf = dbr_unpacki(buf, &body->sess_open.hbint)))
            goto fail1;
        break;
    case DBR_SESS_CLOSE:
        break;
    case DBR_SESS_LOGON:
        if (!(buf = dbr_unpackl(buf, &body->sess_logon.uid)))
            goto fail1;
        break;
    case DBR_SESS_LOGOFF:
        if (!(buf = dbr_unpackl(buf, &body->sess_logoff.uid)))
            goto fail1;
        break;
    case DBR_STATUS_REP:
        buf = dbr_unpackf(buf, STATUS_REP_FORMAT,
                          &body->status_rep.num,
                          DBR_ERRMSG_MAX, body->status_rep.msg);
        break;
    case DBR_ACCNT_LIST_REP:
        if (!(buf = dbr_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_accnt(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, DBR_ENTITY_ACCNT, dbr_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = dbr_queue_first(&q);
        break;
    case DBR_CONTR_LIST_REP:
        if (!(buf = dbr_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_contr(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, DBR_ENTITY_CONTR, dbr_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = dbr_queue_first(&q);
        break;
    case DBR_USER_LIST_REP:
    case DBR_GROUP_LIST_REP:
        if (!(buf = dbr_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_memb(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, DBR_ENTITY_MEMB, dbr_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = dbr_queue_first(&q);
        break;
    case DBR_ORDER_LIST_REP:
        if (!(buf = dbr_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_order(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, DBR_ENTITY_ORDER, dbr_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = dbr_queue_first(&q);
        break;
    case DBR_EXEC_LIST_REP:
        if (!(buf = dbr_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_exec(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, DBR_ENTITY_EXEC, dbr_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = dbr_queue_first(&q);
        break;
    case DBR_POSN_LIST_REP:
        if (!(buf = dbr_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_posn(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, DBR_ENTITY_POSN, dbr_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = dbr_queue_first(&q);
        break;
    case DBR_VIEW_LIST_REP:
        if (!(buf = dbr_unpackz(buf, &body->view_list_rep.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->view_list_rep.count_; ++i) {
            if (!(buf = read_view(buf, pool, &q))) {
                free_view_list(dbr_queue_first(&q), pool);
                goto fail1;
            }
        }
        body->view_list_rep.first = dbr_queue_first(&q);
        break;
    case DBR_EXEC_REP:
        // Exec.
        body->exec_rep.exec = elm_pool_alloc_exec(pool);
        if (!body->exec_rep.exec)
            goto fail1;
        dbr_exec_init(body->exec_rep.exec);

        if (!(buf = elm_proto_read_exec(buf, body->exec_rep.exec))) {
            dbr_exec_decref(body->exec_rep.exec, pool);
            goto fail1;
        }
        break;
    case DBR_POSN_REP:
        // Posn.
        body->posn_rep.posn = elm_pool_alloc_posn(pool);
        if (!body->posn_rep.posn)
            goto fail1;
        dbr_posn_init(body->posn_rep.posn);

        if (!(buf = elm_proto_read_posn(buf, body->posn_rep.posn))) {
            elm_pool_free_posn(pool, body->posn_rep.posn);
            goto fail1;
        }
        break;
    case DBR_PLACE_ORDER_REQ:
        if (!(buf = dbr_unpackf(buf, PLACE_ORDER_REQ_FORMAT,
                                &body->place_order_req.uid,
                                &body->place_order_req.gid,
                                &body->place_order_req.cid,
                                &body->place_order_req.settl_day,
                                DBR_REF_MAX, body->place_order_req.ref,
                                &body->place_order_req.action,
                                &body->place_order_req.ticks,
                                &body->place_order_req.lots,
                                &body->place_order_req.min_lots)))
            goto fail1;
        break;
    case DBR_REVISE_ORDER_ID_REQ:
        if (!(buf = dbr_unpackf(buf, REVISE_ORDER_ID_REQ_FORMAT,
                                &body->revise_order_id_req.uid,
                                &body->revise_order_id_req.id,
                                &body->revise_order_id_req.lots)))
            goto fail1;
        break;
    case DBR_REVISE_ORDER_REF_REQ:
        if (!(buf = dbr_unpackf(buf, REVISE_ORDER_REF_REQ_FORMAT,
                                &body->revise_order_ref_req.uid,
                                DBR_REF_MAX, body->revise_order_ref_req.ref,
                                &body->revise_order_ref_req.lots)))
            goto fail1;
        break;
    case DBR_CANCEL_ORDER_ID_REQ:
        if (!(buf = dbr_unpackf(buf, CANCEL_ORDER_ID_REQ_FORMAT,
                                &body->cancel_order_id_req.uid,
                                &body->cancel_order_id_req.id)))
            goto fail1;
        break;
    case DBR_CANCEL_ORDER_REF_REQ:
        if (!(buf = dbr_unpackf(buf, CANCEL_ORDER_REF_REQ_FORMAT,
                                &body->cancel_order_ref_req.uid,
                                DBR_REF_MAX, body->cancel_order_ref_req.ref)))
            goto fail1;
        break;
    case DBR_ACK_TRADE_REQ:
        if (!(buf = dbr_unpackf(buf, ACK_TRADE_REQ_FORMAT,
                                &body->ack_trade_req.uid,
                                &body->ack_trade_req.id)))
            goto fail1;
        break;
    case DBR_READ_ENTITY_REQ:
        if (!(buf = dbr_unpacki(buf, &body->read_entity_req.type)))
            goto fail1;
        break;
    case DBR_INSERT_EXEC_LIST_REQ:
        if (!(buf = dbr_unpackz(buf, &body->insert_exec_list_req.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->insert_exec_list_req.count_; ++i) {
            if (!(buf = read_exec(buf, pool, &q))) {
                elm_pool_free_entity_list(pool, DBR_ENTITY_EXEC, dbr_queue_first(&q));
                goto fail1;
            }
        }
        body->insert_exec_list_req.first = dbr_queue_first(&q);
        break;
    case DBR_INSERT_EXEC_REQ:
        body->insert_exec_req.exec = elm_pool_alloc_exec(pool);
        if (!body->insert_exec_req.exec)
            goto fail1;
        dbr_exec_init(body->exec_rep.exec);

        if (!(buf = elm_proto_read_exec(buf, body->insert_exec_req.exec))) {
            dbr_exec_decref(body->insert_exec_req.exec, pool);
            goto fail1;
        }
        break;
    case DBR_UPDATE_EXEC_REQ:
        if (!(buf = dbr_unpackf(buf, UPDATE_EXEC_REQ_FORMAT,
                                &body->update_exec_req.id,
                                &body->update_exec_req.modified)))
            goto fail1;
        break;
    default:
        dbr_err_setf(DBR_EIO, "invalid body-type '%d'", type);
        goto fail1;
    }
    return buf;
 fail1:
    return NULL;
}

DBR_EXTERN DbrBool
elm_send_body(void* sock, struct DbrBody* body, DbrBool enriched)
{
    const size_t len = dbr_body_len(body, enriched);
    char buf[len];

    if (!elm_write_body(buf, body, enriched)) {
        dbr_err_set(DBR_EIO, "elm_write_body() failed");
        goto fail1;
    }
    if (zmq_send(sock, buf, len, 0) != len) {
        dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

DBR_EXTERN DbrBool
elm_send_msg(void* sock, const DbrUuid uuid, struct DbrBody* body, DbrBool enriched)
{
    if (zmq_send(sock, uuid, 16, ZMQ_SNDMORE) != 16) {
        dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    return dbr_send_body(sock, body, enriched);
 fail1:
    return DBR_FALSE;
}

DBR_EXTERN DbrBool
elm_recv_body(void* sock, DbrPool pool, struct DbrBody* body)
{
    zmq_msg_t zmsg;
    if (zmq_msg_init(&zmsg) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_msg_init() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    if (zmq_msg_recv(&zmsg, sock, 0) < 0) {
        const int num = zmq_errno() == EINTR ? DBR_EINTR : DBR_EIO;
        dbr_err_setf(num, "zmq_msg_recv() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    if (!elm_read_body(zmq_msg_data(&zmsg), pool, body)) {
        dbr_err_set(DBR_EIO, "elm_read_body() failed");
        goto fail2;
    }
    zmq_msg_close(&zmsg);
    return DBR_TRUE;
 fail2:
    zmq_msg_close(&zmsg);
 fail1:
    return DBR_FALSE;
}

DBR_EXTERN DbrBool
elm_recv_msg(void* sock, DbrPool pool, struct DbrMsg* msg)
{
    zmq_msg_t zmsg;
    if (zmq_msg_init(&zmsg) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_msg_init() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    if (zmq_msg_recv(&zmsg, sock, 0) < 0) {
        const int num = zmq_errno() == EINTR ? DBR_EINTR : DBR_EIO;
        dbr_err_setf(num, "zmq_msg_recv() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    assert(zmq_msg_size(&zmsg) == 16);
    __builtin_memcpy(msg->uuid, zmq_msg_data(&zmsg), 16);
    zmq_msg_close(&zmsg);
    return elm_recv_body(sock, pool, &msg->body);
 fail2:
    zmq_msg_close(&zmsg);
 fail1:
    return DBR_FALSE;
}

DBR_API size_t
dbr_body_len(struct DbrBody* body, DbrBool enriched)
{
    return elm_body_len(body, enriched);
}

DBR_API char*
dbr_write_body(char* buf, const struct DbrBody* body, DbrBool enriched)
{
    return elm_write_body(buf, body, enriched);
}

DBR_API const char*
dbr_read_body(const char* buf, DbrPool pool, struct DbrBody* body)
{
    return elm_read_body(buf, pool, body);
}

DBR_API DbrBool
dbr_send_body(void* sock, struct DbrBody* body, DbrBool enriched)
{
    return elm_send_body(sock, body, enriched);
}

DBR_API DbrBool
dbr_send_msg(void* sock, const DbrUuid uuid, struct DbrBody* body, DbrBool enriched)
{
    return elm_send_msg(sock, uuid, body, enriched);
}

DBR_API DbrBool
dbr_recv_body(void* sock, DbrPool pool, struct DbrBody* body)
{
    return elm_recv_body(sock, pool, body);
}

DBR_API DbrBool
dbr_recv_msg(void* sock, DbrPool pool, struct DbrMsg* msg)
{
    return elm_recv_msg(sock, pool, msg);
}
