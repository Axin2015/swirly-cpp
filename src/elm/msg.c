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
#define _XOPEN_SOURCE 700 // strnlen()
#include <dbr/msg.h>

#include <dbr/err.h>
#include <dbr/pack.h>
#include <dbr/proto.h>
#include <dbr/queue.h>
#include <dbr/refcount.h>

#include <zmq.h>

#include <stdlib.h> // abort()
#include <string.h>

static const char STATUS_REP_FORMAT[] = "is";
static const char PLACE_ORDER_REQ_FORMAT[] = "mmisilll";
static const char REVISE_ORDER_ID_REQ_FORMAT[] = "ll";
static const char REVISE_ORDER_REF_REQ_FORMAT[] = "sl";
static const char UPDATE_EXEC_REQ_FORMAT[] = "li";

static void
free_view_list(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrView* view = dbr_shared_view_entry(node);
        node = node->next;
        dbr_pool_free_view(pool, view);
    }
}

static const char*
read_trader(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    dbr_rec_init(rec);

    if (!(buf = dbr_read_trader(buf, rec))) {
        dbr_pool_free_rec(pool, rec);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &rec->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_accnt(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    dbr_rec_init(rec);

    if (!(buf = dbr_read_accnt(buf, rec))) {
        dbr_pool_free_rec(pool, rec);
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
    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    dbr_rec_init(rec);

    if (!(buf = dbr_read_contr(buf, rec))) {
        dbr_pool_free_rec(pool, rec);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &rec->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_order(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrOrder* order = dbr_pool_alloc_order(pool);
    if (!order)
        goto fail1;
    dbr_order_init(order);

    if (!(buf = dbr_read_order(buf, order))) {
        dbr_pool_free_order(pool, order);
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
    struct DbrExec* exec = dbr_pool_alloc_exec(pool);
    if (!exec)
        goto fail1;
    dbr_exec_init(exec);

    if (!(buf = dbr_read_exec(buf, exec))) {
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
read_memb(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrMemb* memb = dbr_pool_alloc_memb(pool);
    if (!memb)
        goto fail1;
    dbr_memb_init(memb);

    if (!(buf = dbr_read_memb(buf, memb))) {
        dbr_pool_free_memb(pool, memb);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &memb->shared_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_posn(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrPosn* posn = dbr_pool_alloc_posn(pool);
    if (!posn)
        goto fail1;
    dbr_posn_init(posn);

    if (!(buf = dbr_read_posn(buf, posn))) {
        dbr_pool_free_posn(pool, posn);
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
    struct DbrView* view = dbr_pool_alloc_view(pool);
    if (!view)
        goto fail1;
    dbr_view_init(view);

    if (!(buf = dbr_read_view(buf, view))) {
        dbr_pool_free_view(pool, view);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &view->shared_node_);
    return buf;
 fail1:
    return NULL;
}

DBR_API size_t
dbr_body_len(struct DbrBody* body, DbrBool enriched)
{
    size_t n = dbr_packlenl(body->req_id);
    n += dbr_packleni(body->type);
    switch (body->type) {
    case DBR_SESS_LOGON:
        n += dbr_packleni(body->sess_logon.hbint);
        break;
    case DBR_SESS_LOGOFF:
    case DBR_SESS_HEARTBT:
        break;
    case DBR_STATUS_REP:
        n += dbr_packlenf(STATUS_REP_FORMAT,
                          body->status_rep.num,
                          DBR_ERRMSG_MAX, body->status_rep.msg);
        break;
    case DBR_TRADER_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrRec* rec = dbr_shared_rec_entry(node);
            n += dbr_trader_len(rec);
            ++body->entity_list_rep.count_;
        }
        n += dbr_packlenz(body->entity_list_rep.count_);
        break;
    case DBR_ACCNT_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrRec* rec = dbr_shared_rec_entry(node);
            n += dbr_accnt_len(rec);
            ++body->entity_list_rep.count_;
        }
        n += dbr_packlenz(body->entity_list_rep.count_);
        break;
    case DBR_CONTR_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrRec* rec = dbr_shared_rec_entry(node);
            n += dbr_contr_len(rec);
            ++body->entity_list_rep.count_;
        }
        n += dbr_packlenz(body->entity_list_rep.count_);
        break;
    case DBR_ORDER_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrOrder* order = dbr_shared_order_entry(node);
            n += dbr_order_len(order, enriched);
            ++body->entity_list_rep.count_;
        }
        n += dbr_packlenz(body->entity_list_rep.count_);
        break;
    case DBR_EXEC_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrExec* exec = dbr_shared_exec_entry(node);
            n += dbr_exec_len(exec, enriched);
            ++body->entity_list_rep.count_;
        }
        n += dbr_packlenz(body->entity_list_rep.count_);
        break;
    case DBR_MEMB_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrMemb* memb = dbr_shared_memb_entry(node);
            n += dbr_memb_len(memb, enriched);
            ++body->entity_list_rep.count_;
        }
        n += dbr_packlenz(body->entity_list_rep.count_);
        break;
    case DBR_POSN_LIST_REP:
        body->entity_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrPosn* posn = dbr_shared_posn_entry(node);
            n += dbr_posn_len(posn, enriched);
            ++body->entity_list_rep.count_;
        }
        n += dbr_packlenz(body->entity_list_rep.count_);
        break;
    case DBR_VIEW_LIST_REP:
        body->view_list_rep.count_ = 0;
        for (struct DbrSlNode* node = body->view_list_rep.first; node; node = node->next) {
            struct DbrView* view = dbr_shared_view_entry(node);
            n += dbr_view_len(view, enriched);
            ++body->view_list_rep.count_;
        }
        n += dbr_packlenz(body->view_list_rep.count_);
        break;
    case DBR_EXEC_REP:
        n += dbr_exec_len(body->exec_rep.exec, enriched);
        break;
    case DBR_POSN_REP:
        n += dbr_posn_len(body->posn_rep.posn, enriched);
        break;
    case DBR_PLACE_ORDER_REQ:
        n += dbr_packlenf(PLACE_ORDER_REQ_FORMAT,
                          body->place_order_req.accnt,
                          body->place_order_req.contr,
                          body->place_order_req.settl_date,
                          DBR_REF_MAX, body->place_order_req.ref,
                          body->place_order_req.action,
                          body->place_order_req.ticks,
                          body->place_order_req.lots,
                          body->place_order_req.min_lots);
        break;
    case DBR_REVISE_ORDER_ID_REQ:
        n += dbr_packlenf(REVISE_ORDER_ID_REQ_FORMAT,
                          body->revise_order_id_req.id,
                          body->revise_order_id_req.lots);
        break;
    case DBR_REVISE_ORDER_REF_REQ:
        n += dbr_packlenf(REVISE_ORDER_REF_REQ_FORMAT,
                          DBR_REF_MAX, body->revise_order_ref_req.ref,
                          body->revise_order_ref_req.lots);
        break;
    case DBR_CANCEL_ORDER_ID_REQ:
        n += dbr_packlenl(body->cancel_order_id_req.id);
        break;
    case DBR_CANCEL_ORDER_REF_REQ:
        n += dbr_packlens(body->cancel_order_ref_req.ref, DBR_REF_MAX);
        break;
    case DBR_ACK_TRADE_REQ:
        n += dbr_packlenl(body->ack_trade_req.id);
        break;
    case DBR_READ_ENTITY_REQ:
        n += dbr_packleni(body->read_entity_req.type);
        break;
    case DBR_INSERT_EXEC_LIST_REQ:
        body->insert_exec_list_req.count_ = 0;
        for (struct DbrSlNode* node = body->insert_exec_list_req.first; node; node = node->next) {
            struct DbrExec* exec = dbr_shared_exec_entry(node);
            n += dbr_exec_len(exec, enriched);
            ++body->insert_exec_list_req.count_;
        }
        n += dbr_packlenz(body->insert_exec_list_req.count_);
        break;
    case DBR_INSERT_EXEC_REQ:
        n += dbr_exec_len(body->insert_exec_req.exec, enriched);
        break;
    case DBR_UPDATE_EXEC_REQ:
        n += dbr_packlenf(UPDATE_EXEC_REQ_FORMAT,
                          body->update_exec_req.id,
                          body->update_exec_req.modified);
        break;
    default:
        abort();
    }
    return n;
}

DBR_API char*
dbr_write_body(char* buf, const struct DbrBody* body, DbrBool enriched)
{
    buf = dbr_packl(buf, body->req_id);
    buf = dbr_packi(buf, body->type);
    switch (body->type) {
    case DBR_SESS_LOGON:
        buf = dbr_packi(buf, body->sess_logon.hbint);
        break;
    case DBR_SESS_LOGOFF:
    case DBR_SESS_HEARTBT:
        break;
    case DBR_STATUS_REP:
        buf = dbr_packf(buf, STATUS_REP_FORMAT,
                        body->status_rep.num,
                        DBR_ERRMSG_MAX, body->status_rep.msg);
        break;
    case DBR_TRADER_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrRec* rec = dbr_shared_rec_entry(node);
            buf = dbr_write_trader(buf, rec);
        }
        break;
    case DBR_ACCNT_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrRec* rec = dbr_shared_rec_entry(node);
            buf = dbr_write_accnt(buf, rec);
        }
        break;
    case DBR_CONTR_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrRec* rec = dbr_shared_rec_entry(node);
            buf = dbr_write_contr(buf, rec);
        }
        break;
    case DBR_ORDER_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrOrder* order = dbr_shared_order_entry(node);
            buf = dbr_write_order(buf, order, enriched);
        }
        break;
    case DBR_EXEC_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrExec* exec = dbr_shared_exec_entry(node);
            buf = dbr_write_exec(buf, exec, enriched);
        }
        break;
    case DBR_MEMB_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrMemb* memb = dbr_shared_memb_entry(node);
            buf = dbr_write_memb(buf, memb, enriched);
        }
        break;
    case DBR_POSN_LIST_REP:
        buf = dbr_packz(buf, body->entity_list_rep.count_);
        for (struct DbrSlNode* node = body->entity_list_rep.first; node; node = node->next) {
            struct DbrPosn* posn = dbr_shared_posn_entry(node);
            buf = dbr_write_posn(buf, posn, enriched);
        }
        break;
    case DBR_VIEW_LIST_REP:
        buf = dbr_packz(buf, body->view_list_rep.count_);
        for (struct DbrSlNode* node = body->view_list_rep.first; node; node = node->next) {
            struct DbrView* view = dbr_shared_view_entry(node);
            buf = dbr_write_view(buf, view, enriched);
        }
        break;
    case DBR_EXEC_REP:
        buf = dbr_write_exec(buf, body->exec_rep.exec, enriched);
        break;
    case DBR_POSN_REP:
        buf = dbr_write_posn(buf, body->posn_rep.posn, enriched);
        break;
    case DBR_PLACE_ORDER_REQ:
        buf = dbr_packf(buf, PLACE_ORDER_REQ_FORMAT,
                        body->place_order_req.accnt,
                        body->place_order_req.contr,
                        body->place_order_req.settl_date,
                        DBR_REF_MAX, body->place_order_req.ref,
                        body->place_order_req.action,
                        body->place_order_req.ticks,
                        body->place_order_req.lots,
                        body->place_order_req.min_lots);
        break;
    case DBR_REVISE_ORDER_ID_REQ:
        buf = dbr_packf(buf, REVISE_ORDER_ID_REQ_FORMAT,
                        body->revise_order_id_req.id,
                        body->revise_order_id_req.lots);
        break;
    case DBR_REVISE_ORDER_REF_REQ:
        buf = dbr_packf(buf, REVISE_ORDER_REF_REQ_FORMAT,
                        DBR_REF_MAX, body->revise_order_ref_req.ref,
                        body->revise_order_ref_req.lots);
        break;
    case DBR_CANCEL_ORDER_ID_REQ:
        buf = dbr_packl(buf, body->cancel_order_id_req.id);
        break;
    case DBR_CANCEL_ORDER_REF_REQ:
        buf = dbr_packs(buf, body->cancel_order_ref_req.ref, DBR_REF_MAX);
        break;
    case DBR_ACK_TRADE_REQ:
        buf = dbr_packl(buf, body->ack_trade_req.id);
        break;
    case DBR_READ_ENTITY_REQ:
        buf = dbr_packi(buf, body->read_entity_req.type);
        break;
    case DBR_INSERT_EXEC_LIST_REQ:
        buf = dbr_packz(buf, body->insert_exec_list_req.count_);
        for (struct DbrSlNode* node = body->insert_exec_list_req.first; node; node = node->next) {
            struct DbrExec* exec = dbr_shared_exec_entry(node);
            buf = dbr_write_exec(buf, exec, enriched);
        }
        break;
    case DBR_INSERT_EXEC_REQ:
        buf = dbr_write_exec(buf, body->insert_exec_req.exec, enriched);
        break;
    case DBR_UPDATE_EXEC_REQ:
        buf = dbr_packf(buf, UPDATE_EXEC_REQ_FORMAT,
                        body->update_exec_req.id,
                        body->update_exec_req.modified);
        break;
    }
    return buf;
}

DBR_API const char*
dbr_read_body(const char* buf, DbrPool pool, struct DbrBody* body)
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
    case DBR_SESS_LOGON:
        if (!(buf = dbr_unpacki(buf, &body->sess_logon.hbint)))
            goto fail1;
        break;
    case DBR_SESS_LOGOFF:
    case DBR_SESS_HEARTBT:
        break;
    case DBR_STATUS_REP:
        buf = dbr_unpackf(buf, STATUS_REP_FORMAT,
                          &body->status_rep.num,
                          DBR_ERRMSG_MAX, body->status_rep.msg);
        break;
    case DBR_TRADER_LIST_REP:
        if (!(buf = dbr_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_trader(buf, pool, &q))) {
                dbr_pool_free_entity_list(pool, DBR_TRADER, dbr_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = dbr_queue_first(&q);
        break;
    case DBR_ACCNT_LIST_REP:
        if (!(buf = dbr_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_accnt(buf, pool, &q))) {
                dbr_pool_free_entity_list(pool, DBR_ACCNT, dbr_queue_first(&q));
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
                dbr_pool_free_entity_list(pool, DBR_CONTR, dbr_queue_first(&q));
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
                dbr_pool_free_entity_list(pool, DBR_ORDER, dbr_queue_first(&q));
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
                dbr_pool_free_entity_list(pool, DBR_EXEC, dbr_queue_first(&q));
                goto fail1;
            }
        }
        body->entity_list_rep.first = dbr_queue_first(&q);
        break;
    case DBR_MEMB_LIST_REP:
        if (!(buf = dbr_unpackz(buf, &body->entity_list_rep.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->entity_list_rep.count_; ++i) {
            if (!(buf = read_memb(buf, pool, &q))) {
                dbr_pool_free_entity_list(pool, DBR_MEMB, dbr_queue_first(&q));
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
                dbr_pool_free_entity_list(pool, DBR_POSN, dbr_queue_first(&q));
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
        body->exec_rep.exec = dbr_pool_alloc_exec(pool);
        if (!body->exec_rep.exec)
            goto fail1;
        dbr_exec_init(body->exec_rep.exec);

        if (!(buf = dbr_read_exec(buf, body->exec_rep.exec))) {
            dbr_exec_decref(body->exec_rep.exec, pool);
            goto fail1;
        }
        break;
    case DBR_POSN_REP:
        // Posn.
        body->posn_rep.posn = dbr_pool_alloc_posn(pool);
        if (!body->posn_rep.posn)
            goto fail1;
        dbr_posn_init(body->posn_rep.posn);

        if (!(buf = dbr_read_posn(buf, body->posn_rep.posn))) {
            dbr_pool_free_posn(pool, body->posn_rep.posn);
            goto fail1;
        }
        break;
    case DBR_PLACE_ORDER_REQ:
        if (!(buf = dbr_unpackf(buf, PLACE_ORDER_REQ_FORMAT,
                                body->place_order_req.accnt,
                                body->place_order_req.contr,
                                &body->place_order_req.settl_date,
                                DBR_REF_MAX, body->place_order_req.ref,
                                &body->place_order_req.action,
                                &body->place_order_req.ticks,
                                &body->place_order_req.lots,
                                &body->place_order_req.min_lots)))
            goto fail1;
        break;
    case DBR_REVISE_ORDER_ID_REQ:
        if (!(buf = dbr_unpackf(buf, REVISE_ORDER_ID_REQ_FORMAT,
                                &body->revise_order_id_req.id,
                                &body->revise_order_id_req.lots)))
            goto fail1;
        break;
    case DBR_REVISE_ORDER_REF_REQ:
        if (!(buf = dbr_unpackf(buf, REVISE_ORDER_REF_REQ_FORMAT,
                                DBR_REF_MAX, body->revise_order_ref_req.ref,
                                &body->revise_order_ref_req.lots)))
            goto fail1;
        break;
    case DBR_CANCEL_ORDER_ID_REQ:
        if (!(buf = dbr_unpackl(buf, &body->cancel_order_id_req.id)))
            goto fail1;
        break;
    case DBR_CANCEL_ORDER_REF_REQ:
        if (!(buf = dbr_unpacks(buf, body->cancel_order_ref_req.ref, DBR_REF_MAX)))
            goto fail1;
        break;
    case DBR_ACK_TRADE_REQ:
        if (!(buf = dbr_unpackl(buf, &body->ack_trade_req.id)))
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
                dbr_pool_free_entity_list(pool, DBR_EXEC, dbr_queue_first(&q));
                goto fail1;
            }
        }
        body->insert_exec_list_req.first = dbr_queue_first(&q);
        break;
    case DBR_INSERT_EXEC_REQ:
        body->insert_exec_req.exec = dbr_pool_alloc_exec(pool);
        if (!body->insert_exec_req.exec)
            goto fail1;
        dbr_exec_init(body->exec_rep.exec);

        if (!(buf = dbr_read_exec(buf, body->insert_exec_req.exec))) {
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

DBR_API DbrBool
dbr_send_body(void* sock, struct DbrBody* body, DbrBool enriched)
{
    const size_t len = dbr_body_len(body, enriched);
    char buf[len];

    if (!dbr_write_body(buf, body, enriched)) {
        dbr_err_set(DBR_EIO, "dbr_write_body() failed");
        goto fail1;
    }
    if (zmq_send(sock, buf, len, 0) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

DBR_API DbrBool
dbr_send_msg(void* sock, const char* trader, struct DbrBody* body, DbrBool enriched)
{
    if (zmq_send(sock, trader, strnlen(trader, DBR_MNEM_MAX), ZMQ_SNDMORE) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    return dbr_send_body(sock, body, enriched);
 fail1:
    return DBR_FALSE;
}

DBR_API DbrBool
dbr_recv_body(void* sock, DbrPool pool, struct DbrBody* body)
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
    if (!dbr_read_body(zmq_msg_data(&zmsg), pool, body)) {
        dbr_err_set(DBR_EIO, "dbr_read_body() failed");
        goto fail2;
    }
    zmq_msg_close(&zmsg);
    return DBR_TRUE;
 fail2:
    zmq_msg_close(&zmsg);
 fail1:
    return DBR_FALSE;
}

DBR_API DbrBool
dbr_recv_msg(void* sock, DbrPool pool, struct DbrMsg* msg)
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
    const size_t size = zmq_msg_size(&zmsg);
    if (size < DBR_MNEM_MAX) {
        __builtin_memcpy(msg->head.trader, zmq_msg_data(&zmsg), size);
        msg->head.trader[size] = '\0';
    } else {
        assert(size == DBR_MNEM_MAX);
        __builtin_memcpy(msg->head.trader, zmq_msg_data(&zmsg), DBR_MNEM_MAX);
    }
    zmq_msg_close(&zmsg);
    return dbr_recv_body(sock, pool, &msg->body);
 fail2:
    zmq_msg_close(&zmsg);
 fail1:
    return DBR_FALSE;
}
