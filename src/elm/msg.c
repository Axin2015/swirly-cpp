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
#define _XOPEN_SOURCE 700 // strnlen()
#include <dbr/msg.h>

#include <dbr/err.h>
#include <dbr/pack.h>
#include <dbr/proto.h>
#include <dbr/queue.h>

#include <zmq.h>

#include <stdlib.h> // abort()
#include <string.h>

static const char STATUS_REP_FORMAT[] = "is";
static const char PLACE_ORDER_REQ_FORMAT[] = "mmisillll";
static const char REVISE_ORDER_ID_REQ_FORMAT[] = "ll";
static const char REVISE_ORDER_REF_REQ_FORMAT[] = "sl";

static void
free_entity_posns(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrPosn* posn = dbr_entity_posn_entry(node);
        node = node->next;
        dbr_pool_free_posn(pool, posn);
    }
}

static void
free_entity_execs(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrExec* exec = dbr_entity_exec_entry(node);
        node = node->next;
        dbr_pool_free_exec(pool, exec);
    }
}

static void
free_trans_stmts(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrStmt* stmt = dbr_trans_stmt_entry(node);
        node = node->next;
        dbr_pool_free_stmt(pool, stmt);
    }
}

static const char*
read_entity_trader(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    if (!(buf = dbr_read_trader(buf, rec))) {
        dbr_pool_free_rec(pool, rec);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &rec->entity_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_entity_accnt(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    if (!(buf = dbr_read_accnt(buf, rec))) {
        dbr_pool_free_rec(pool, rec);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &rec->entity_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_entity_contr(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    if (!(buf = dbr_read_contr(buf, rec))) {
        dbr_pool_free_rec(pool, rec);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &rec->entity_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_entity_order(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrOrder* order = dbr_pool_alloc_order(pool);
    if (!order)
        goto fail1;
    if (!(buf = dbr_read_order(buf, order))) {
        dbr_pool_free_order(pool, order);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &order->entity_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_entity_exec(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrExec* exec = dbr_pool_alloc_exec(pool);
    if (!exec)
        goto fail1;
    if (!(buf = dbr_read_exec(buf, exec))) {
        dbr_pool_free_exec(pool, exec);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &exec->entity_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_entity_memb(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrMemb* memb = dbr_pool_alloc_memb(pool);
    if (!memb)
        goto fail1;
    if (!(buf = dbr_read_memb(buf, memb))) {
        dbr_pool_free_memb(pool, memb);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &memb->entity_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_entity_posn(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrPosn* posn = dbr_pool_alloc_posn(pool);
    if (!posn)
        goto fail1;
    if (!(buf = dbr_read_posn(buf, posn))) {
        dbr_pool_free_posn(pool, posn);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &posn->entity_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_trans_stmt(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrStmt* stmt = dbr_pool_alloc_stmt(pool);
    if (!stmt)
        goto fail1;
    if (!(buf = dbr_read_stmt(buf, stmt))) {
        dbr_pool_free_stmt(pool, stmt);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &stmt->trans_node_);
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
    case DBR_SESS_HB:
        break;
    case DBR_STATUS_REP:
        n += dbr_packlenf(STATUS_REP_FORMAT,
                          body->status_rep.num,
                          DBR_ERRMSG_MAX, body->status_rep.msg);
        break;
    case DBR_ENTITY_REP:
        n += dbr_packleni(body->entity_rep.type);
        body->entity_rep.count_ = 0;
        switch (body->entity_rep.type) {
        case DBR_TRADER:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_entity_rec_entry(node);
                n += dbr_trader_len(rec);
                ++body->entity_rep.count_;
            }
            break;
        case DBR_ACCNT:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_entity_rec_entry(node);
                n += dbr_accnt_len(rec);
                ++body->entity_rep.count_;
            }
            break;
        case DBR_CONTR:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_entity_rec_entry(node);
                n += dbr_contr_len(rec);
                ++body->entity_rep.count_;
            }
            break;
        case DBR_ORDER:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrOrder* order = dbr_entity_order_entry(node);
                n += dbr_order_len(order, enriched);
                ++body->entity_rep.count_;
            }
            break;
        case DBR_EXEC:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrExec* exec = dbr_entity_exec_entry(node);
                n += dbr_exec_len(exec, enriched);
                ++body->entity_rep.count_;
            }
            break;
        case DBR_MEMB:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrMemb* memb = dbr_entity_memb_entry(node);
                n += dbr_memb_len(memb, enriched);
                ++body->entity_rep.count_;
            }
            break;
        case DBR_POSN:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrPosn* posn = dbr_entity_posn_entry(node);
                n += dbr_posn_len(posn, enriched);
                ++body->entity_rep.count_;
            }
            break;
        default:
            abort();
        }
        n += dbr_packlenz(body->entity_rep.count_);
        break;
    case DBR_CYCLE_REP:
        n += dbr_order_len(body->cycle_rep.new_order, enriched);
        body->cycle_rep.exec_count_ = 0;
        for (struct DbrSlNode* node = body->cycle_rep.first_exec;
             node; node = node->next) {
            struct DbrExec* exec = dbr_entity_exec_entry(node);
            n += dbr_exec_len(exec, enriched);
            ++body->cycle_rep.exec_count_;
        }
        n += dbr_packlenz(body->cycle_rep.exec_count_);
        body->cycle_rep.posn_count_ = 0;
        for (struct DbrSlNode* node = body->cycle_rep.first_posn;
             node; node = node->next) {
            struct DbrPosn* posn = dbr_entity_posn_entry(node);
            n += dbr_posn_len(posn, enriched);
            ++body->cycle_rep.posn_count_;
        }
        n += dbr_packlenz(body->cycle_rep.posn_count_);
        break;
    case DBR_ORDER_REP:
        n += dbr_order_len(body->order_rep.order, enriched);
        break;
    case DBR_LOGON_REQ:
    case DBR_LOGOFF_REQ:
        break;
    case DBR_READ_ENTITY_REQ:
        n += dbr_packleni(body->read_entity_req.type);
        break;
    case DBR_SESS_ENTITY_REQ:
        n += dbr_packleni(body->sess_entity_req.type);
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
    case DBR_ARCHIVE_ORDER_REQ:
        n += dbr_packlenl(body->archive_order_req.id);
        break;
    case DBR_ARCHIVE_TRADE_REQ:
        n += dbr_packlenl(body->archive_trade_req.id);
        break;
    case DBR_WRITE_TRANS_REQ:
        body->write_trans_req.count_ = 0;
        for (struct DbrSlNode* node = body->write_trans_req.first; node; node = node->next) {
            struct DbrStmt* stmt = dbr_trans_stmt_entry(node);
            n += dbr_stmt_len(stmt);
            ++body->write_trans_req.count_;
        }
        n += dbr_packlenz(body->write_trans_req.count_);
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
    case DBR_SESS_HB:
        break;
    case DBR_STATUS_REP:
        buf = dbr_packf(buf, STATUS_REP_FORMAT,
                        body->status_rep.num,
                        DBR_ERRMSG_MAX, body->status_rep.msg);
        break;
    case DBR_ENTITY_REP:
        buf = dbr_packi(buf, body->entity_rep.type);
        buf = dbr_packz(buf, body->entity_rep.count_);
        switch (body->entity_rep.type) {
        case DBR_TRADER:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_entity_rec_entry(node);
                buf = dbr_write_trader(buf, rec);
            }
            break;
        case DBR_ACCNT:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_entity_rec_entry(node);
                buf = dbr_write_accnt(buf, rec);
            }
            break;
        case DBR_CONTR:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_entity_rec_entry(node);
                buf = dbr_write_contr(buf, rec);
            }
            break;
        case DBR_ORDER:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrOrder* order = dbr_entity_order_entry(node);
                buf = dbr_write_order(buf, order, enriched);
            }
            break;
        case DBR_EXEC:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrExec* exec = dbr_entity_exec_entry(node);
                buf = dbr_write_exec(buf, exec, enriched);
            }
            break;
        case DBR_MEMB:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrMemb* memb = dbr_entity_memb_entry(node);
                buf = dbr_write_memb(buf, memb, enriched);
            }
            break;
        case DBR_POSN:
            for (struct DbrSlNode* node = body->entity_rep.first; node; node = node->next) {
                struct DbrPosn* posn = dbr_entity_posn_entry(node);
                buf = dbr_write_posn(buf, posn, enriched);
            }
            break;
        default:
            abort();
        }
        break;
    case DBR_CYCLE_REP:
        buf = dbr_write_order(buf, body->cycle_rep.new_order, enriched);
        buf = dbr_packz(buf, body->cycle_rep.exec_count_);
        for (struct DbrSlNode* node = body->cycle_rep.first_exec;
             node; node = node->next) {
            struct DbrExec* exec = dbr_entity_exec_entry(node);
            buf = dbr_write_exec(buf, exec, enriched);
        }
        buf = dbr_packz(buf, body->cycle_rep.posn_count_);
        for (struct DbrSlNode* node = body->cycle_rep.first_posn;
             node; node = node->next) {
            struct DbrPosn* posn = dbr_entity_posn_entry(node);
            buf = dbr_write_posn(buf, posn, enriched);
        }
        break;
    case DBR_ORDER_REP:
        buf = dbr_write_order(buf, body->order_rep.order, enriched);
        break;
    case DBR_LOGON_REQ:
    case DBR_LOGOFF_REQ:
        break;
    case DBR_READ_ENTITY_REQ:
        buf = dbr_packi(buf, body->read_entity_req.type);
        break;
    case DBR_SESS_ENTITY_REQ:
        buf = dbr_packi(buf, body->sess_entity_req.type);
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
    case DBR_ARCHIVE_ORDER_REQ:
        buf = dbr_packl(buf, body->archive_order_req.id);
        break;
    case DBR_ARCHIVE_TRADE_REQ:
        buf = dbr_packl(buf, body->archive_trade_req.id);
        break;
    case DBR_WRITE_TRANS_REQ:
        buf = dbr_packz(buf, body->write_trans_req.count_);
        for (struct DbrSlNode* node = body->write_trans_req.first; node; node = node->next) {
            struct DbrStmt* stmt = dbr_trans_stmt_entry(node);
            buf = dbr_write_stmt(buf, stmt);
        }
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
    case DBR_SESS_HB:
        break;
    case DBR_STATUS_REP:
        buf = dbr_unpackf(buf, STATUS_REP_FORMAT,
                          &body->status_rep.num,
                          DBR_ERRMSG_MAX, body->status_rep.msg);
        break;
    case DBR_ENTITY_REP:
        if (!(buf = dbr_unpacki(buf, &body->entity_rep.type)))
            goto fail1;
        if (!(buf = dbr_unpackz(buf, &body->entity_rep.count_)))
            goto fail1;
        dbr_queue_init(&q);
        switch (body->entity_rep.type) {
        case DBR_TRADER:
            for (size_t i = 0; i < body->entity_rep.count_; ++i) {
                if (!(buf = read_entity_trader(buf, pool, &q))) {
                    dbr_pool_free_entities(pool, DBR_TRADER, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            break;
        case DBR_ACCNT:
            for (size_t i = 0; i < body->entity_rep.count_; ++i) {
                if (!(buf = read_entity_accnt(buf, pool, &q))) {
                    dbr_pool_free_entities(pool, DBR_ACCNT, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            break;
        case DBR_CONTR:
            for (size_t i = 0; i < body->entity_rep.count_; ++i) {
                if (!(buf = read_entity_contr(buf, pool, &q))) {
                    dbr_pool_free_entities(pool, DBR_CONTR, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            break;
        case DBR_ORDER:
            for (size_t i = 0; i < body->entity_rep.count_; ++i) {
                if (!(buf = read_entity_order(buf, pool, &q))) {
                    dbr_pool_free_entities(pool, DBR_ORDER, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            break;
        case DBR_EXEC:
            for (size_t i = 0; i < body->entity_rep.count_; ++i) {
                if (!(buf = read_entity_exec(buf, pool, &q))) {
                    dbr_pool_free_entities(pool, DBR_EXEC, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            break;
        case DBR_MEMB:
            for (size_t i = 0; i < body->entity_rep.count_; ++i) {
                if (!(buf = read_entity_memb(buf, pool, &q))) {
                    dbr_pool_free_entities(pool, DBR_MEMB, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            break;
        case DBR_POSN:
            for (size_t i = 0; i < body->entity_rep.count_; ++i) {
                if (!(buf = read_entity_posn(buf, pool, &q))) {
                    dbr_pool_free_entities(pool, DBR_POSN, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            break;
        default:
            dbr_err_setf(DBR_EIO, "invalid entity-type '%d'", type);
            goto fail1;
        }
        body->entity_rep.first = dbr_queue_first(&q);
        break;
    case DBR_CYCLE_REP:
        // Order.
        body->cycle_rep.new_order = dbr_pool_alloc_order(pool);
        if (!body->cycle_rep.new_order)
            goto fail1;
        if (!(buf = dbr_read_order(buf, body->cycle_rep.new_order))) {
            dbr_pool_free_order(pool, body->cycle_rep.new_order);
            goto fail1;
        }
        // Execs.
        if (!(buf = dbr_unpackz(buf, &body->cycle_rep.exec_count_))) {
            free_entity_posns(body->cycle_rep.first_posn, pool);
            dbr_pool_free_order(pool, body->cycle_rep.new_order);
            goto fail1;
        }
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->cycle_rep.exec_count_; ++i) {
            if (!(buf = read_entity_exec(buf, pool, &q))) {
                free_entity_execs(body->cycle_rep.first_exec, pool);
                free_entity_posns(body->cycle_rep.first_posn, pool);
                dbr_pool_free_order(pool, body->cycle_rep.new_order);
                goto fail1;
            }
        }
        body->cycle_rep.first_exec = dbr_queue_first(&q);
        // Posns.
        if (!(buf = dbr_unpackz(buf, &body->cycle_rep.posn_count_))) {
            dbr_pool_free_order(pool, body->cycle_rep.new_order);
            goto fail1;
        }
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->cycle_rep.posn_count_; ++i) {
            if (!(buf = read_entity_posn(buf, pool, &q))) {
                free_entity_posns(body->cycle_rep.first_posn, pool);
                dbr_pool_free_order(pool, body->cycle_rep.new_order);
                goto fail1;
            }
        }
        body->cycle_rep.first_posn = dbr_queue_first(&q);
        break;
    case DBR_ORDER_REP:
        // Order.
        body->order_rep.order = dbr_pool_alloc_order(pool);
        if (!body->order_rep.order)
            goto fail1;
        if (!(buf = dbr_read_order(buf, body->order_rep.order))) {
            dbr_pool_free_order(pool, body->order_rep.order);
            goto fail1;
        }
        break;
    case DBR_LOGON_REQ:
    case DBR_LOGOFF_REQ:
        break;
    case DBR_READ_ENTITY_REQ:
        if (!(buf = dbr_unpacki(buf, &body->read_entity_req.type)))
            goto fail1;
        break;
    case DBR_SESS_ENTITY_REQ:
        if (!(buf = dbr_unpacki(buf, &body->sess_entity_req.type)))
            goto fail1;
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
    case DBR_ARCHIVE_ORDER_REQ:
        if (!(buf = dbr_unpackl(buf, &body->archive_order_req.id)))
            goto fail1;
        break;
    case DBR_ARCHIVE_TRADE_REQ:
        if (!(buf = dbr_unpackl(buf, &body->archive_trade_req.id)))
            goto fail1;
        break;
    case DBR_WRITE_TRANS_REQ:
        if (!(buf = dbr_unpackz(buf, &body->write_trans_req.count_)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < body->write_trans_req.count_; ++i) {
            if (!(buf = read_trans_stmt(buf, pool, &q))) {
                free_trans_stmts(dbr_queue_first(&q), pool);
                goto fail1;
            }
        }
        body->write_trans_req.first = dbr_queue_first(&q);
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
    } else
        __builtin_memcpy(msg->head.trader, zmq_msg_data(&zmsg), DBR_MNEM_MAX);
    zmq_msg_close(&zmsg);
    return dbr_recv_body(sock, pool, &msg->body);
 fail2:
    zmq_msg_close(&zmsg);
 fail1:
    return DBR_FALSE;
}
