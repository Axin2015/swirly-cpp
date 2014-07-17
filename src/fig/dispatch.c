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
#include <dbr/fig/dispatch.h>

#include "accnt.h"
#include "async.h"
#include "clnt.h"

#include <dbr/fig/sess.h>

#include <dbr/elm/msg.h>

#include <dbr/ash/err.h>
#include <dbr/ash/log.h>
#include <dbr/ash/util.h>

#include <limits.h>

static inline struct DbrRec*
get_id(struct FigCache* cache, int type, DbrIden id)
{
    struct DbrSlNode* node = fig_cache_find_rec_id(cache, type, id);
    assert(node != FIG_CACHE_END_REC);
    return dbr_shared_rec_entry(node);
}

static inline struct DbrPerm*
enrich_perm(struct FigCache* cache, struct DbrPerm* perm)
{
    perm->trader.rec = get_id(cache, DBR_ENTITY_ACCNT, perm->trader.id_only);
    perm->giveup.rec = get_id(cache, DBR_ENTITY_ACCNT, perm->giveup.id_only);
    return perm;
}

static inline struct DbrOrder*
enrich_order(struct FigCache* cache, struct DbrOrder* order)
{
    order->c.trader.rec = get_id(cache, DBR_ENTITY_ACCNT, order->c.trader.id_only);
    order->c.giveup.rec = get_id(cache, DBR_ENTITY_ACCNT, order->c.giveup.id_only);
    order->c.contr.rec = get_id(cache, DBR_ENTITY_CONTR, order->c.contr.id_only);
    return order;
}

static inline struct DbrExec*
enrich_exec(struct FigCache* cache, struct DbrExec* exec)
{
    exec->c.trader.rec = get_id(cache, DBR_ENTITY_ACCNT, exec->c.trader.id_only);
    exec->c.giveup.rec = get_id(cache, DBR_ENTITY_ACCNT, exec->c.giveup.id_only);
    exec->c.contr.rec = get_id(cache, DBR_ENTITY_CONTR, exec->c.contr.id_only);
    if (exec->cpty.id_only)
        exec->cpty.rec = get_id(cache, DBR_ENTITY_ACCNT, exec->cpty.id_only);
    else
        exec->cpty.rec = NULL;
    return exec;
}

static inline struct DbrPosn*
enrich_posn(struct FigCache* cache, struct DbrPosn* posn)
{
    posn->accnt.rec = get_id(cache, DBR_ENTITY_ACCNT, posn->accnt.id_only);
    posn->contr.rec = get_id(cache, DBR_ENTITY_CONTR, posn->contr.id_only);
    return posn;
}

static inline struct DbrView*
enrich_view(struct FigCache* cache, struct DbrView* view)
{
    view->contr.rec = get_id(cache, DBR_ENTITY_CONTR, view->contr.id_only);
    return view;
}

static DbrAccnt
get_accnt(DbrClnt clnt, DbrIden aid)
{
    struct DbrRec* arec = get_id(&clnt->cache, DBR_ENTITY_ACCNT, aid);
    assert(arec->accnt.state);
    return arec->accnt.state;
}

static void
emplace_trader_list(DbrClnt clnt, struct DbrSlNode* first)
{
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrPerm* perm = enrich_perm(&clnt->cache, dbr_shared_perm_entry(node));
        DbrAccnt giveup = perm->giveup.rec->accnt.state;
        assert(giveup);
        // Transfer ownership.
        fig_accnt_emplace_trader(giveup, perm);
    }
}

static void
emplace_giveup_list(DbrClnt clnt, struct DbrSlNode* first)
{
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrPerm* perm = enrich_perm(&clnt->cache, dbr_shared_perm_entry(node));
        DbrAccnt trader = perm->trader.rec->accnt.state;
        assert(trader);
        // Transfer ownership.
        fig_accnt_emplace_giveup(trader, perm);
    }
}

static void
emplace_order_list(DbrClnt clnt, struct DbrSlNode* first)
{
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrOrder* order = enrich_order(&clnt->cache, dbr_shared_order_entry(node));
        DbrAccnt trader = order->c.trader.rec->accnt.state;
        assert(trader);
        // Transfer ownership.
        fig_accnt_emplace_order(trader, order);
    }
}

static void
emplace_exec_list(DbrClnt clnt, struct DbrSlNode* first)
{
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrExec* exec = enrich_exec(&clnt->cache, dbr_shared_exec_entry(node));
        assert(exec->c.state == DBR_STATE_TRADE);
        DbrAccnt trader = exec->c.trader.rec->accnt.state;
        assert(trader);
        // Transfer ownership.
        fig_accnt_insert_trade(trader, exec);
        dbr_exec_decref(exec, clnt->pool);
    }
}

static DbrBool
emplace_posn_list(DbrClnt clnt, struct DbrSlNode* first)
{
    DbrBool nomem = DBR_FALSE;
    for (struct DbrSlNode* node = first; node; ) {
        struct DbrPosn* posn = enrich_posn(&clnt->cache, dbr_shared_posn_entry(node));
        node = node->next;
        // Transfer ownership.
        // All accounts that trader is member of are created in emplace_perms().
        DbrAccnt accnt = fig_accnt_lazy(posn->accnt.rec, &clnt->ordidx, clnt->pool);
        if (dbr_likely(accnt)) {
            fig_accnt_emplace_posn(accnt, posn);
        } else {
            dbr_pool_free_posn(clnt->pool, posn);
            nomem = DBR_TRUE;
        }
    }
    if (dbr_unlikely(nomem)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

static struct DbrOrder*
create_order(DbrClnt clnt, struct DbrExec* exec)
{
    struct DbrOrder* order = dbr_pool_alloc_order(clnt->pool);
    if (!order)
        return NULL;
    dbr_order_init(order);

    order->level = NULL;
    order->id = exec->order;
    __builtin_memcpy(&order->c, &exec->c, sizeof(struct DbrCommon));
    order->created = exec->created;
    order->modified = exec->created;
    return order;
}

static DbrBool
apply_new(DbrClnt clnt, struct DbrExec* exec)
{
    struct DbrOrder* order = create_order(clnt, exec);
    if (!order)
        return DBR_FALSE;
    DbrAccnt trader = order->c.trader.rec->accnt.state;
    assert(trader);
    // Transfer ownership.
    fig_accnt_emplace_order(trader, order);
    return DBR_TRUE;
}

static DbrBool
apply_update(DbrClnt clnt, struct DbrExec* exec)
{
    DbrAccnt trader = exec->c.trader.rec->accnt.state;
    assert(trader);
    struct DbrRbNode* node = fig_accnt_find_order_id(trader, exec->order);
    if (!node) {
        dbr_err_setf(DBR_EINVAL, "no such order '%ld'", exec->order);
        return DBR_FALSE;
    }

    struct DbrOrder* order = dbr_accnt_order_entry(node);
    order->c.state = exec->c.state;
    order->c.lots = exec->c.lots;
    order->c.resd = exec->c.resd;
    order->c.exec = exec->c.exec;
    order->c.last_ticks = exec->c.last_ticks;
    order->c.last_lots = exec->c.last_lots;
    order->modified = exec->created;

    if (exec->c.state == DBR_STATE_TRADE) {
        // Transfer ownership.
        fig_accnt_insert_trade(trader, exec);
    }
    return DBR_TRUE;
}

static struct DbrPosn*
apply_posnup(DbrClnt clnt, struct DbrPosn* posn)
{
    DbrAccnt accnt = fig_accnt_lazy(posn->accnt.rec, &clnt->ordidx, clnt->pool);
    if (dbr_unlikely(!accnt)) {
        dbr_pool_free_posn(accnt->pool, posn);
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return NULL;
    }
    posn = fig_accnt_update_posn(accnt, posn);
    return posn;
}

static struct DbrView*
apply_viewup(DbrClnt clnt, struct DbrView* view)
{
    const DbrKey key = dbr_view_key(view->contr.rec->id, view->settl_day);
    struct DbrRbNode* node = dbr_tree_insert(&clnt->views, key, &view->clnt_node_);
    if (node != &view->clnt_node_) {

        struct DbrView* exist = dbr_clnt_view_entry(node);

        // Drop if node already exists and overwrite is not set.
        if (dbr_unlikely(view->created < exist->created)) {
            dbr_log_warn("dropped stale view");
            dbr_pool_free_view(clnt->pool, view);
            return NULL;
        }

        // Update existing position.

        assert(exist->contr.rec == view->contr.rec);
        assert(exist->settl_day == view->settl_day);

        for (size_t i = 0; i < DBR_LEVEL_MAX; ++i) {
            exist->bid_ticks[i] = view->bid_ticks[i];
            exist->bid_lots[i] = view->bid_lots[i];
            exist->bid_count[i] = view->bid_count[i];
            exist->offer_ticks[i] = view->offer_ticks[i];
            exist->offer_lots[i] = view->offer_lots[i];
            exist->offer_count[i] = view->offer_count[i];
        }
        exist->created = view->created;

        dbr_pool_free_view(clnt->pool, view);
        view = exist;
    }
    return view;
}

static void
apply_views(DbrClnt clnt, struct DbrSlNode* first, DbrHandler handler)
{
    // In the following example, the snapshot should overwrite the T+0 delta, but not the T+2 delta:
    // T+0: EURUSD delta
    // T+2: USDJPY delta
    // T+1: snapshot

    for (struct DbrSlNode* node = first; node; ) {
        struct DbrView* view = dbr_shared_view_entry(node);
        node = node->next;
        // Transfer ownership or free.
        enrich_view(&clnt->cache, view);
        view = apply_viewup(clnt, view);
        if (dbr_likely(view))
            dbr_handler_on_view(handler, clnt, view);
    }
    dbr_handler_on_flush(handler, clnt);
}

DBR_API DbrBool
dbr_clnt_dispatch(DbrClnt clnt, DbrMillis ms, DbrHandler handler)
{
    assert(ms >= 0);

    if (clnt->state == FIG_CLOSED) {
        dbr_err_set(DBR_EBUSY, "client is closed");
        goto fail1;
    }

    DbrMillis now = dbr_millis();
    const DbrMillis absms = now + ms;
    // At least one iteration.
    do {
        if (clnt->state == (FIG_DELTA_WAIT | FIG_CLOSE_WAIT)) {
            dbr_log_debug1("close immediate");
            clnt->state = FIG_CLOSED;
            fig_clnt_log_state(clnt->state);
            dbr_handler_on_close(handler, clnt);
            goto done;
        }
        const struct DbrElem* elem;
        while ((elem = dbr_prioq_top(&clnt->prioq))) {
            if (elem->key > now) {
                // Millis until next timeout.
                ms = dbr_min(absms, elem->key) - now;
                break;
            }
            // Internal timeout.
            const DbrKey key = elem->key;
            const DbrIden id = elem->id;
            dbr_prioq_pop(&clnt->prioq);

            if (id == FIG_HBTMR) {
                // Cannot fail due to pop.
                dbr_prioq_push(&clnt->prioq, id, key + clnt->sess.hbint);
                struct DbrBody body = { .req_id = 0, .type = DBR_SESS_HEARTBT };
                if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
                    goto fail1;
                // Next heartbeat may have already expired.
            } else if (id == FIG_MDTMR) {
                if (!(clnt->state & (FIG_DELTA_WAIT | FIG_CLOSE_WAIT))) {
                    fig_clnt_sess_reset(clnt);
                    dbr_handler_on_reset(handler, clnt);
                }
                // Cannot fail due to pop.
                dbr_prioq_push(&clnt->prioq, id, LONG_MAX);
                dbr_err_setf(DBR_ETIMEOUT, "market-data socket timeout");
                goto fail1;
            } else if (id == FIG_TRTMR) {
                if (!(clnt->state & (FIG_DELTA_WAIT | FIG_CLOSE_WAIT))) {
                    fig_clnt_sess_reset(clnt);
                    dbr_handler_on_reset(handler, clnt);
                }
                // Cannot fail due to pop.
                dbr_prioq_push(&clnt->prioq, id, LONG_MAX);
                dbr_err_setf(DBR_ETIMEOUT, "transaction socket timeout");
                goto fail1;
            } else if (id == clnt->close_id) {
                dbr_log_debug1("close timeout");
                clnt->state = FIG_CLOSED;
                fig_clnt_log_state(clnt->state);
                dbr_handler_on_close(handler, clnt);
                goto done;
            } else {
                // Assumed that these "top-half" handlers do not block.
                dbr_handler_on_timeout(handler, clnt, id);
            }
        }

        // From the zmq_poll() man page:

        // For each zmq_pollitem_t item, zmq_poll() shall first clear the revents member, and then
        // indicate any requested events that have occurred by setting the bit corresponding to the
        // event condition in the revents member.

        // Note also that zmq_poll() is level-triggered.

        const int nevents = zmq_poll(clnt->items, FIG_NSOCK, ms);
        if (nevents < 0) {
            dbr_err_setf(DBR_EIO, "zmq_poll() failed: %s", zmq_strerror(zmq_errno()));
            goto fail1;
        }
        // Current time after slow operation.
        now = dbr_millis();
        if (nevents == 0)
            continue;

        struct DbrBody body;
        if ((clnt->items[FIG_TRSOCK].revents & ZMQ_POLLIN)) {

            if (!dbr_recv_body(clnt->trsock, clnt->pool, &body))
                goto fail1;

            if (body.req_id > 0)
                dbr_prioq_remove(&clnt->prioq, body.req_id);

            dbr_prioq_replace(&clnt->prioq, FIG_TRTMR, now + FIG_TRTMOUT);

            switch (body.type) {
            case DBR_SESS_OPEN:
                dbr_log_debug1("open message");
                clnt->sess.hbint = body.sess_open.hbint;
                clnt->state &= ~FIG_OPEN_WAIT;
                clnt->state |= (FIG_REC_WAIT | FIG_SNAP_WAIT);
                fig_clnt_log_state(clnt->state);
                if (!dbr_prioq_push(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint)
                    || !dbr_prioq_push(&clnt->prioq, FIG_TRTMR, now + FIG_TRTMOUT))
                    goto fail1;
                break;
            case DBR_SESS_CLOSE:
                dbr_log_debug1("close message");
                clnt->state = FIG_CLOSED;
                fig_clnt_log_state(clnt->state);
                dbr_handler_on_close(handler, clnt);
                goto done;
            case DBR_SESS_LOGON:
                dbr_log_debug1("logon message");
                dbr_sess_logon(&clnt->sess, get_accnt(clnt, body.sess_logon.aid));
                dbr_handler_on_logon(handler, clnt, body.req_id, body.sess_logon.aid);
                break;
            case DBR_SESS_LOGOFF:
                dbr_log_debug1("logoff message");
                dbr_handler_on_logoff(handler, clnt, body.req_id, body.sess_logoff.aid);
                {
                    DbrAccnt accnt = get_accnt(clnt, body.sess_logoff.aid);
                    dbr_sess_logoff_and_reset(&clnt->sess, accnt);
                }
                break;
            case DBR_SESS_HEARTBT:
                break;
            case DBR_STATUS_REP:
                dbr_log_debug1("status message");
                dbr_handler_on_status(handler, clnt, body.req_id, body.status_rep.num,
                                      body.status_rep.msg);
                break;
            case DBR_ACCNT_LIST_REP:
                dbr_log_debug1("accnt-list message");
                clnt->state &= ~FIG_ACCNT_WAIT;
                fig_clnt_log_state(clnt->state);
                fig_cache_emplace_rec_list(&clnt->cache, DBR_ENTITY_ACCNT,
                                           body.entity_list_rep.first, body.entity_list_rep.count_);
                if (clnt->state == FIG_READY)
                    dbr_handler_on_ready(handler, clnt);
                break;
            case DBR_CONTR_LIST_REP:
                dbr_log_debug1("contr-list message");
                clnt->state &= ~FIG_CONTR_WAIT;
                fig_clnt_log_state(clnt->state);
                fig_cache_emplace_rec_list(&clnt->cache, DBR_ENTITY_CONTR,
                                           body.entity_list_rep.first, body.entity_list_rep.count_);
                if (clnt->state == FIG_READY)
                    dbr_handler_on_ready(handler, clnt);
                break;
            case DBR_TRADER_LIST_REP:
                dbr_log_debug1("trader-list message");
                emplace_trader_list(clnt, body.entity_list_rep.first);
                break;
            case DBR_GIVEUP_LIST_REP:
                dbr_log_debug1("giveup-list message");
                emplace_giveup_list(clnt, body.entity_list_rep.first);
                break;
            case DBR_ORDER_LIST_REP:
                dbr_log_debug1("order-list message");
                emplace_order_list(clnt, body.entity_list_rep.first);
                break;
            case DBR_EXEC_LIST_REP:
                dbr_log_debug1("exec-list message");
                emplace_exec_list(clnt, body.entity_list_rep.first);
                break;
            case DBR_POSN_LIST_REP:
                dbr_log_debug1("posn-list message");
                // This function can fail is there is no memory available for a lazily created
                // account.
                if (dbr_unlikely(!emplace_posn_list(clnt, body.entity_list_rep.first))) {
                    fig_clnt_sess_reset(clnt);
                    goto fail1;
                }
                break;
            case DBR_VIEW_LIST_REP:
                dbr_log_debug1("view-list message");
                clnt->state &= ~FIG_SNAP_WAIT;
                fig_clnt_log_state(clnt->state);
                apply_views(clnt, body.view_list_rep.first, handler);
                if (clnt->state == FIG_READY)
                    dbr_handler_on_ready(handler, clnt);
                break;
            case DBR_EXEC_REP:
                dbr_log_debug1("exec message");
                enrich_exec(&clnt->cache, body.exec_rep.exec);
                switch (body.exec_rep.exec->c.state) {
                case DBR_STATE_NEW:
                    apply_new(clnt, body.exec_rep.exec);
                    break;
                case DBR_STATE_REVISE:
                case DBR_STATE_CANCEL:
                case DBR_STATE_TRADE:
                    apply_update(clnt, body.exec_rep.exec);
                    break;
                }
                dbr_handler_on_exec(handler, clnt, body.req_id, body.exec_rep.exec);
                if (dbr_exec_done(body.exec_rep.exec)) {
                    struct DbrExec* exec = body.exec_rep.exec;
                    DbrAccnt trader = exec->c.trader.rec->accnt.state;
                    assert(trader);
                    struct DbrOrder* order = fig_accnt_release_order_id(trader, exec->order);
                    if (order)
                        dbr_pool_free_order(clnt->pool, order);
                }
                dbr_exec_decref(body.exec_rep.exec, clnt->pool);
                break;
            case DBR_POSN_REP:
                dbr_log_debug1("posn message");
                enrich_posn(&clnt->cache, body.posn_rep.posn);
                // This function can fail is there is no memory available for a lazily created
                // account.
                body.posn_rep.posn = apply_posnup(clnt, body.posn_rep.posn);
                if (dbr_unlikely(!body.posn_rep.posn))
                    goto fail1;
                dbr_handler_on_posn(handler, clnt, body.posn_rep.posn);
                break;
            default:
                dbr_err_setf(DBR_EIO, "unknown body-type '%d'", body.type);
                goto fail1;
            }
        }

        if ((clnt->items[FIG_MDSOCK].revents & ZMQ_POLLIN)) {

            if (!dbr_recv_body(clnt->mdsock, clnt->pool, &body))
                goto fail1;

            dbr_prioq_replace(&clnt->prioq, FIG_MDTMR, now + FIG_MDTMOUT);

            switch (body.type) {
            case DBR_VIEW_LIST_REP:
                if (dbr_unlikely(clnt->state & FIG_DELTA_WAIT)) {
                    if (clnt->state == (FIG_DELTA_WAIT | FIG_CLOSE_WAIT)) {
                        dbr_log_debug1("close immediate");
                        clnt->state = FIG_CLOSED;
                        fig_clnt_log_state(clnt->state);
                        dbr_handler_on_close(handler, clnt);
                        goto done;
                    }
                    // Otherwise proceed with open request.
                    clnt->state &= ~FIG_DELTA_WAIT;
                    if (fig_clnt_sess_open(clnt, now) < 0)
                        goto fail1;
                } else
                    apply_views(clnt, body.view_list_rep.first, handler);
                break;
            default:
                dbr_err_setf(DBR_EIO, "unknown body-type '%d'", body.type);
                goto fail1;
            }
        }

        if ((clnt->items[FIG_ASOCK].revents & ZMQ_POLLIN)) {

            void* val;
            if (!fig_async_recv(clnt->asock, &val))
                goto fail1;

            if (dbr_unlikely(val == (void*)~0)) {
                if (fig_clnt_sess_close(clnt, now) < 0)
                    goto fail1;
                val = NULL;
            } else
                val = dbr_handler_on_async(handler, clnt, val);

            if (!fig_async_send(clnt->asock, val))
                goto fail1;
        }
    } while (now < absms);
 done:
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}
