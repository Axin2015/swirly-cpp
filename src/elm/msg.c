#include <dbr/msg.h>

#include <dbr/err.h>
#include <dbr/model.h>
#include <dbr/pack.h>
#include <dbr/proto.h>
#include <dbr/queue.h>

#include <zmq.h>

#include <stdlib.h> // abort()

static size_t
dbr_msg_len(struct DbrMsg* msg)
{
    size_t n = dbr_packleni(msg->type);
    switch (msg->type) {
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
    default:
        abort();
    }
    return n;
}

DBR_EXTERN char*
dbr_write_msg(char* buf, const struct DbrMsg* msg)
{
    buf = dbr_packi(buf, msg->type);
    switch (msg->type) {
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
    default:
        abort();
    }
    return buf;
}

DBR_EXTERN const char*
dbr_read_msg(DbrPool pool, const char* buf, struct DbrMsg* msg)
{
    int type;
    if (!(buf = dbr_unpacki(buf, &type)))
        goto fail1;
    msg->type = type;
    struct DbrQueue q;
    switch (type) {
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
            for (int i = 0; i < msg->read_entity_rep.count; ++i) {
                struct DbrRec* rec = dbr_pool_alloc_rec(pool);
                if (!rec) {
                    dbr_pool_free_list(pool, DBR_TRADER, dbr_queue_first(&q));
                    goto fail1;
                }
                if (!(buf = dbr_read_trader(buf, rec))) {
                    dbr_pool_free_rec(pool, rec);
                    dbr_pool_free_list(pool, DBR_TRADER, dbr_queue_first(&q));
                    goto fail1;
                }
                dbr_queue_insert_back(&q, &rec->model_node_);
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_ACCNT:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (int i = 0; i < msg->read_entity_rep.count; ++i) {
                struct DbrRec* rec = dbr_pool_alloc_rec(pool);
                if (!rec) {
                    dbr_pool_free_list(pool, DBR_ACCNT, dbr_queue_first(&q));
                    goto fail1;
                }
                if (!(buf = dbr_read_accnt(buf, rec))) {
                    dbr_pool_free_rec(pool, rec);
                    dbr_pool_free_list(pool, DBR_ACCNT, dbr_queue_first(&q));
                    goto fail1;
                }
                dbr_queue_insert_back(&q, &rec->model_node_);
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_CONTR:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (int i = 0; i < msg->read_entity_rep.count; ++i) {
                struct DbrRec* rec = dbr_pool_alloc_rec(pool);
                if (!rec) {
                    dbr_pool_free_list(pool, DBR_CONTR, dbr_queue_first(&q));
                    goto fail1;
                }
                if (!(buf = dbr_read_contr(buf, rec))) {
                    dbr_pool_free_rec(pool, rec);
                    dbr_pool_free_list(pool, DBR_CONTR, dbr_queue_first(&q));
                    goto fail1;
                }
                dbr_queue_insert_back(&q, &rec->model_node_);
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_ORDER:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (int i = 0; i < msg->read_entity_rep.count; ++i) {
                struct DbrOrder* order = dbr_pool_alloc_order(pool);
                if (!order) {
                    dbr_pool_free_list(pool, DBR_ORDER, dbr_queue_first(&q));
                    goto fail1;
                }
                if (!(buf = dbr_read_order(buf, order))) {
                    dbr_pool_free_order(pool, order);
                    dbr_pool_free_list(pool, DBR_ORDER, dbr_queue_first(&q));
                    goto fail1;
                }
                dbr_queue_insert_back(&q, &order->model_node_);
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_MEMB:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (int i = 0; i < msg->read_entity_rep.count; ++i) {
                struct DbrMemb* memb = dbr_pool_alloc_memb(pool);
                if (!memb) {
                    dbr_pool_free_list(pool, DBR_MEMB, dbr_queue_first(&q));
                    goto fail1;
                }
                if (!(buf = dbr_read_memb(buf, memb))) {
                    dbr_pool_free_memb(pool, memb);
                    dbr_pool_free_list(pool, DBR_MEMB, dbr_queue_first(&q));
                    goto fail1;
                }
                dbr_queue_insert_back(&q, &memb->model_node_);
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_TRADE:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (int i = 0; i < msg->read_entity_rep.count; ++i) {
                struct DbrTrade* trade = dbr_pool_alloc_trade(pool);
                if (!trade) {
                    dbr_pool_free_list(pool, DBR_TRADE, dbr_queue_first(&q));
                    goto fail1;
                }
                if (!(buf = dbr_read_trade(buf, trade))) {
                    dbr_pool_free_trade(pool, trade);
                    dbr_pool_free_list(pool, DBR_TRADE, dbr_queue_first(&q));
                    goto fail1;
                }
                dbr_queue_insert_back(&q, &trade->model_node_);
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        case DBR_POSN:
            if (!(buf = dbr_unpackz(buf, &msg->read_entity_rep.count)))
                goto fail1;
            dbr_queue_init(&q);
            for (int i = 0; i < msg->read_entity_rep.count; ++i) {
                struct DbrPosn* posn = dbr_pool_alloc_posn(pool);
                if (!posn) {
                    dbr_pool_free_list(pool, DBR_POSN, dbr_queue_first(&q));
                    goto fail1;
                }
                if (!(buf = dbr_read_posn(buf, posn))) {
                    dbr_pool_free_posn(pool, posn);
                    dbr_pool_free_list(pool, DBR_POSN, dbr_queue_first(&q));
                    goto fail1;
                }
                dbr_queue_insert_back(&q, &posn->model_node_);
            }
            msg->read_entity_rep.first = dbr_queue_first(&q);
            break;
        default:
            dbr_err_set(DBR_EIO, "invalid entity-type '%d'", type);
            goto fail1;
        }
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
        dbr_err_set(DBR_EIO, "zmq_msg_recv() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    if (!dbr_read_msg(pool, zmq_msg_data(&zmsg), msg)) {
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
