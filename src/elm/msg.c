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
#include <dbr/msg.h>

#include <dbr/err.h>
#include <dbr/model.h>
#include <dbr/pack.h>
#include <dbr/proto.h>
#include <dbr/queue.h>

#include <zmq.h>

#include <stdlib.h> // abort()

static const char STATUS_ERR_FORMAT[] = "is";
static const char PLACE_ORDER_FORMAT[] = "mmmisillll";

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

static void
free_result_posns(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrPosn* posn = dbr_result_posn_entry(node);
        node = node->next;
        dbr_pool_free_posn(pool, posn);
    }
}

static void
free_result_trades(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrTrade* trade = dbr_result_trade_entry(node);
        node = node->next;
        dbr_pool_free_trade(pool, trade);
    }
}

static const char*
read_model_trader(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    if (!(buf = dbr_read_trader(buf, rec))) {
        dbr_pool_free_rec(pool, rec);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &rec->model_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_model_accnt(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    if (!(buf = dbr_read_accnt(buf, rec))) {
        dbr_pool_free_rec(pool, rec);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &rec->model_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_model_contr(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    if (!rec)
        goto fail1;
    if (!(buf = dbr_read_contr(buf, rec))) {
        dbr_pool_free_rec(pool, rec);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &rec->model_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_model_order(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrOrder* order = dbr_pool_alloc_order(pool);
    if (!order)
        goto fail1;
    if (!(buf = dbr_read_order(buf, order))) {
        dbr_pool_free_order(pool, order);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &order->model_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_model_memb(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrMemb* memb = dbr_pool_alloc_memb(pool);
    if (!memb)
        goto fail1;
    if (!(buf = dbr_read_memb(buf, memb))) {
        dbr_pool_free_memb(pool, memb);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &memb->model_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_model_trade(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrTrade* trade = dbr_pool_alloc_trade(pool);
    if (!trade)
        goto fail1;
    if (!(buf = dbr_read_trade(buf, trade))) {
        dbr_pool_free_trade(pool, trade);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &trade->model_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_model_posn(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrPosn* posn = dbr_pool_alloc_posn(pool);
    if (!posn)
        goto fail1;
    if (!(buf = dbr_read_posn(buf, posn))) {
        dbr_pool_free_posn(pool, posn);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &posn->model_node_);
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

static const char*
read_result_posn(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrPosn* posn = dbr_pool_alloc_posn(pool);
    if (!posn)
        goto fail1;
    if (!(buf = dbr_read_posn(buf, posn))) {
        dbr_pool_free_posn(pool, posn);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &posn->result_node_);
    return buf;
 fail1:
    return NULL;
}

static const char*
read_result_trade(const char* buf, DbrPool pool, struct DbrQueue* queue)
{
    struct DbrTrade* trade = dbr_pool_alloc_trade(pool);
    if (!trade)
        goto fail1;
    if (!(buf = dbr_read_trade(buf, trade))) {
        dbr_pool_free_trade(pool, trade);
        goto fail1;
    }
    dbr_queue_insert_back(queue, &trade->result_node_);
    return buf;
 fail1:
    return NULL;
}

DBR_API size_t
dbr_msg_len(struct DbrMsg* msg)
{
    size_t n = dbr_packleni(msg->type);
    switch (msg->type) {
    case DBR_STATUS_OK:
        break;
    case DBR_STATUS_ERR:
        n += dbr_packlenf(STATUS_ERR_FORMAT,
                          msg->status_err.num,
                          DBR_ERRMSG_MAX, msg->status_err.msg);
        break;
    case DBR_PLACE_ORDER_REQ:
        n += dbr_packlenf(PLACE_ORDER_FORMAT,
                          msg->place_order_req.trader,
                          msg->place_order_req.accnt,
                          msg->place_order_req.contr,
                          msg->place_order_req.settl_date,
                          DBR_REF_MAX, msg->place_order_req.ref,
                          msg->place_order_req.action,
                          msg->place_order_req.ticks,
                          msg->place_order_req.lots,
                          msg->place_order_req.min,
                          msg->place_order_req.flags);
        break;
    case DBR_PLACE_ORDER_REP:
        n += dbr_order_len(msg->place_order_rep.new_order);
        msg->place_order_rep.posn_count = 0;
        for (struct DbrSlNode* node = msg->place_order_rep.first_posn;
             node; node = node->next) {
            struct DbrPosn* posn = dbr_result_posn_entry(node);
            n += dbr_posn_len(posn);
            ++msg->place_order_rep.posn_count;
        }
        n += dbr_packlenz(msg->place_order_rep.posn_count);
        msg->place_order_rep.trade_count = 0;
        for (struct DbrSlNode* node = msg->place_order_rep.first_trade;
             node; node = node->next) {
            struct DbrTrade* trade = dbr_result_trade_entry(node);
            n += dbr_trade_len(trade);
            ++msg->place_order_rep.trade_count;
        }
        n += dbr_packlenz(msg->place_order_rep.trade_count);
        break;
    case DBR_READ_ENTITY_REQ:
        n += dbr_packleni(msg->read_entity_req.type);
        break;
    case DBR_READ_ENTITY_REP:
        n += dbr_packleni(msg->read_entity_rep.type);
        switch (msg->read_entity_rep.type) {
        case DBR_TRADER:
            msg->read_entity_rep.count = 0;
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_model_rec_entry(node);
                n += dbr_trader_len(rec);
                ++msg->read_entity_rep.count;
            }
            n += dbr_packlenz(msg->read_entity_rep.count);
            break;
        case DBR_ACCNT:
            msg->read_entity_rep.count = 0;
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_model_rec_entry(node);
                n += dbr_accnt_len(rec);
                ++msg->read_entity_rep.count;
            }
            n += dbr_packlenz(msg->read_entity_rep.count);
            break;
        case DBR_CONTR:
            msg->read_entity_rep.count = 0;
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_model_rec_entry(node);
                n += dbr_contr_len(rec);
                ++msg->read_entity_rep.count;
            }
            n += dbr_packlenz(msg->read_entity_rep.count);
            break;
        case DBR_ORDER:
            msg->read_entity_rep.count = 0;
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrOrder* order = dbr_model_order_entry(node);
                n += dbr_order_len(order);
                ++msg->read_entity_rep.count;
            }
            n += dbr_packlenz(msg->read_entity_rep.count);
            break;
        case DBR_MEMB:
            msg->read_entity_rep.count = 0;
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrMemb* memb = dbr_model_memb_entry(node);
                n += dbr_memb_len(memb);
                ++msg->read_entity_rep.count;
            }
            n += dbr_packlenz(msg->read_entity_rep.count);
            break;
        case DBR_TRADE:
            msg->read_entity_rep.count = 0;
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrTrade* trade = dbr_model_trade_entry(node);
                n += dbr_trade_len(trade);
                ++msg->read_entity_rep.count;
            }
            n += dbr_packlenz(msg->read_entity_rep.count);
            break;
        case DBR_POSN:
            msg->read_entity_rep.count = 0;
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrPosn* posn = dbr_model_posn_entry(node);
                n += dbr_posn_len(posn);
                ++msg->read_entity_rep.count;
            }
            n += dbr_packlenz(msg->read_entity_rep.count);
            break;
        default:
            abort();
        }
        break;
    case DBR_WRITE_TRANS_REQ:
        msg->write_trans_req.count = 0;
        for (struct DbrSlNode* node = msg->write_trans_req.first; node; node = node->next) {
            struct DbrStmt* stmt = dbr_trans_stmt_entry(node);
            n += dbr_stmt_len(stmt);
            ++msg->write_trans_req.count;
        }
        n += dbr_packlenz(msg->write_trans_req.count);
        break;
    default:
        abort();
    }
    return n;
}

DBR_API char*
dbr_write_msg(char* buf, const struct DbrMsg* msg)
{
    buf = dbr_packi(buf, msg->type);
    switch (msg->type) {
    case DBR_STATUS_OK:
        break;
    case DBR_STATUS_ERR:
        buf = dbr_packf(buf, STATUS_ERR_FORMAT,
                        msg->status_err.num,
                        DBR_ERRMSG_MAX, msg->status_err.msg);
        break;
    case DBR_PLACE_ORDER_REQ:
        buf = dbr_packf(buf, PLACE_ORDER_FORMAT,
                        msg->place_order_req.trader,
                        msg->place_order_req.accnt,
                        msg->place_order_req.contr,
                        msg->place_order_req.settl_date,
                        DBR_REF_MAX, msg->place_order_req.ref,
                        msg->place_order_req.action,
                        msg->place_order_req.ticks,
                        msg->place_order_req.lots,
                        msg->place_order_req.min,
                        msg->place_order_req.flags);
        break;
    case DBR_PLACE_ORDER_REP:
        buf = dbr_write_order(buf, msg->place_order_rep.new_order);
        buf = dbr_packz(buf, msg->place_order_rep.posn_count);
        for (struct DbrSlNode* node = msg->place_order_rep.first_posn;
             node; node = node->next) {
            struct DbrPosn* posn = dbr_result_posn_entry(node);
            buf = dbr_write_posn(buf, posn);
        }
        buf = dbr_packz(buf, msg->place_order_rep.trade_count);
        for (struct DbrSlNode* node = msg->place_order_rep.first_trade;
             node; node = node->next) {
            struct DbrTrade* trade = dbr_result_trade_entry(node);
            buf = dbr_write_trade(buf, trade);
        }
        break;
    case DBR_READ_ENTITY_REQ:
        buf = dbr_packi(buf, msg->read_entity_req.type);
        break;
    case DBR_READ_ENTITY_REP:
        buf = dbr_packi(buf, msg->read_entity_rep.type);
        switch (msg->read_entity_rep.type) {
        case DBR_TRADER:
            buf = dbr_packz(buf, msg->read_entity_rep.count);
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_model_rec_entry(node);
                buf = dbr_write_trader(buf, rec);
            }
            break;
        case DBR_ACCNT:
            buf = dbr_packz(buf, msg->read_entity_rep.count);
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_model_rec_entry(node);
                buf = dbr_write_accnt(buf, rec);
            }
            break;
        case DBR_CONTR:
            buf = dbr_packz(buf, msg->read_entity_rep.count);
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrRec* rec = dbr_model_rec_entry(node);
                buf = dbr_write_contr(buf, rec);
            }
            break;
        case DBR_ORDER:
            buf = dbr_packz(buf, msg->read_entity_rep.count);
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrOrder* order = dbr_model_order_entry(node);
                buf = dbr_write_order(buf, order);
            }
            break;
        case DBR_MEMB:
            buf = dbr_packz(buf, msg->read_entity_rep.count);
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrMemb* memb = dbr_model_memb_entry(node);
                buf = dbr_write_memb(buf, memb);
            }
            break;
        case DBR_TRADE:
            buf = dbr_packz(buf, msg->read_entity_rep.count);
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrTrade* trade = dbr_model_trade_entry(node);
                buf = dbr_write_trade(buf, trade);
            }
            break;
        case DBR_POSN:
            buf = dbr_packz(buf, msg->read_entity_rep.count);
            for (struct DbrSlNode* node = msg->read_entity_rep.first; node; node = node->next) {
                struct DbrPosn* posn = dbr_model_posn_entry(node);
                buf = dbr_write_posn(buf, posn);
            }
            break;
        default:
            abort();
        }
        break;
    case DBR_WRITE_TRANS_REQ:
        buf = dbr_packz(buf, msg->write_trans_req.count);
        for (struct DbrSlNode* node = msg->write_trans_req.first; node; node = node->next) {
            struct DbrStmt* stmt = dbr_trans_stmt_entry(node);
            buf = dbr_write_stmt(buf, stmt);
        }
        break;
    }
    return buf;
}

DBR_API const char*
dbr_read_msg(const char* buf, DbrPool pool, struct DbrMsg* msg)
{
    int type;
    if (!(buf = dbr_unpacki(buf, &type)))
        goto fail1;
    msg->type = type;
    struct DbrQueue q;
    switch (type) {
    case DBR_STATUS_OK:
        break;
    case DBR_STATUS_ERR:
        buf = dbr_unpackf(buf, STATUS_ERR_FORMAT,
                          &msg->status_err.num,
                          DBR_ERRMSG_MAX, msg->status_err.msg);
        break;
    case DBR_PLACE_ORDER_REQ:
        buf = dbr_unpackf(buf, PLACE_ORDER_FORMAT,
                          msg->place_order_req.trader,
                          msg->place_order_req.accnt,
                          msg->place_order_req.contr,
                          &msg->place_order_req.settl_date,
                          DBR_REF_MAX, msg->place_order_req.ref,
                          &msg->place_order_req.action,
                          &msg->place_order_req.ticks,
                          &msg->place_order_req.lots,
                          &msg->place_order_req.min,
                          &msg->place_order_req.flags);
        break;
    case DBR_PLACE_ORDER_REP:
        // Order.
        msg->place_order_rep.new_order = dbr_pool_alloc_order(pool);
        if (!msg->place_order_rep.new_order)
            goto fail1;
        if (!(buf = dbr_read_order(buf, msg->place_order_rep.new_order))) {
            dbr_pool_free_order(pool, msg->place_order_rep.new_order);
            goto fail1;
        }
        // Posns.
        if (!(buf = dbr_unpackz(buf, &msg->place_order_rep.posn_count))) {
            dbr_pool_free_order(pool, msg->place_order_rep.new_order);
            goto fail1;
        }
        dbr_queue_init(&q);
        for (size_t i = 0; i < msg->place_order_rep.posn_count; ++i) {
            if (!read_result_posn(buf, pool, &q)) {
                free_result_posns(msg->place_order_rep.first_posn, pool);
                dbr_pool_free_order(pool, msg->place_order_rep.new_order);
                goto fail1;
            }
        }
        msg->place_order_rep.first_posn = dbr_queue_first(&q);
        // Trades.
        if (!(buf = dbr_unpackz(buf, &msg->place_order_rep.trade_count))) {
            free_result_posns(msg->place_order_rep.first_posn, pool);
            dbr_pool_free_order(pool, msg->place_order_rep.new_order);
            goto fail1;
        }
        dbr_queue_init(&q);
        for (size_t i = 0; i < msg->place_order_rep.trade_count; ++i) {
            if (!read_result_trade(buf, pool, &q)) {
                free_result_trades(msg->place_order_rep.first_trade, pool);
                free_result_posns(msg->place_order_rep.first_posn, pool);
                dbr_pool_free_order(pool, msg->place_order_rep.new_order);
                goto fail1;
            }
        }
        msg->place_order_rep.first_trade = dbr_queue_first(&q);
        break;
    case DBR_READ_ENTITY_REQ:
        if (!(buf = dbr_unpacki(buf, &msg->read_entity_req.type)))
            goto fail1;
        break;
    case DBR_READ_ENTITY_REP:
        if (!(buf = dbr_unpacki(buf, &msg->read_entity_rep.type)))
            goto fail1;
        switch (msg->read_entity_rep.type) {
        case DBR_TRADER:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (size_t i = 0; i < msg->read_entity_rep.count; ++i) {
                if (!read_model_trader(buf, pool, &q)) {
                    dbr_pool_free_list(pool, DBR_TRADER, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_ACCNT:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (size_t i = 0; i < msg->read_entity_rep.count; ++i) {
                if (!read_model_accnt(buf, pool, &q)) {
                    dbr_pool_free_list(pool, DBR_ACCNT, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_CONTR:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (size_t i = 0; i < msg->read_entity_rep.count; ++i) {
                if (!read_model_contr(buf, pool, &q)) {
                    dbr_pool_free_list(pool, DBR_CONTR, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_ORDER:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (size_t i = 0; i < msg->read_entity_rep.count; ++i) {
                if (!read_model_order(buf, pool, &q)) {
                    dbr_pool_free_list(pool, DBR_ORDER, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_MEMB:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (size_t i = 0; i < msg->read_entity_rep.count; ++i) {
                if (!read_model_memb(buf, pool, &q)) {
                    dbr_pool_free_list(pool, DBR_MEMB, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_TRADE:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (size_t i = 0; i < msg->read_entity_rep.count; ++i) {
                if (!read_model_trade(buf, pool, &q)) {
                    dbr_pool_free_list(pool, DBR_TRADE, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_POSN:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (size_t i = 0; i < msg->read_entity_rep.count; ++i) {
                if (!read_model_posn(buf, pool, &q)) {
                    dbr_pool_free_list(pool, DBR_POSN, dbr_queue_first(&q));
                    goto fail1;
                }
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        default:
            dbr_err_set(DBR_EIO, "invalid entity-type '%d'", type);
            goto fail1;
        }
        break;
    case DBR_WRITE_TRANS_REQ:
        if (!(buf = dbr_unpackz(buf, &msg->write_trans_req.count)))
            goto fail1;
        dbr_queue_init(&q);
        for (size_t i = 0; i < msg->write_trans_req.count; ++i) {
            if (!read_trans_stmt(buf, pool, &q)) {
                free_trans_stmts(dbr_queue_first(&q), pool);
                goto fail1;
            }
        }
        msg->write_trans_req.first = dbr_queue_first(&q);
        break;
    default:
        dbr_err_set(DBR_EIO, "invalid msg-type '%d'", type);
        goto fail1;
    }
    return buf;
 fail1:
    return NULL;
}

DBR_EXTERN DbrBool
dbr_recv_msg(void* sock, DbrPool pool, struct DbrMsg* msg)
{
    zmq_msg_t zmsg;
    if (zmq_msg_init(&zmsg) < 0) {
        dbr_err_set(DBR_EIO, "zmq_msg_init() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    if (zmq_msg_recv(&zmsg, sock, 0) < 0) {
        const int num = zmq_errno() == EINTR ? DBR_EINTR : DBR_EIO;
        dbr_err_set(num, "zmq_msg_recv() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    if (!dbr_read_msg(zmq_msg_data(&zmsg), pool, msg)) {
        dbr_err_set(DBR_EIO, "dbr_read_msg() failed");
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
dbr_send_msg(void* sock, struct DbrMsg* msg)
{
    zmq_msg_t zmsg;
    if (zmq_msg_init_size(&zmsg, dbr_msg_len(msg)) < 0) {
        dbr_err_set(DBR_EIO, "zmq_msg_init_size() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    if (!dbr_write_msg(zmq_msg_data(&zmsg), msg)) {
        dbr_err_set(DBR_EIO, "dbr_write_msg() failed");
        goto fail2;
    }
    if (zmq_msg_send(&zmsg, sock, 0) < 0) {
        dbr_err_set(DBR_EIO, "zmq_msg_send() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    zmq_msg_close(&zmsg);
    return DBR_TRUE;
 fail2:
    zmq_msg_close(&zmsg);
 fail1:
    return DBR_FALSE;
}
