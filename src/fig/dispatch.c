/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/fig/dispatch.h>

#include "accnt.h"
#include "async.h"
#include "clnt.h"

#include <sc/fig/sess.h>

#include <sc/elm/msg.h>

#include <sc/ash/err.h>
#include <sc/ash/log.h>
#include <sc/ash/util.h>

#include <limits.h>

static inline struct ScRec*
get_id(struct FigCache* cache, int type, ScIden id)
{
    struct ScSlNode* node = fig_cache_find_rec_id(cache, type, id);
    assert(node != FIG_CACHE_END_REC);
    return sc_shared_rec_entry(node);
}

static inline struct ScPerm*
enrich_perm(struct FigCache* cache, struct ScPerm* perm)
{
    perm->trader.rec = get_id(cache, SC_ENTITY_ACCNT, perm->trader.id_only);
    perm->giveup.rec = get_id(cache, SC_ENTITY_ACCNT, perm->giveup.id_only);
    return perm;
}

static inline struct ScOrder*
enrich_order(struct FigCache* cache, struct ScOrder* order)
{
    order->i.trader.rec = get_id(cache, SC_ENTITY_ACCNT, order->i.trader.id_only);
    order->i.giveup.rec = get_id(cache, SC_ENTITY_ACCNT, order->i.giveup.id_only);
    order->i.contr.rec = get_id(cache, SC_ENTITY_CONTR, order->i.contr.id_only);
    return order;
}

static inline struct ScExec*
enrich_exec(struct FigCache* cache, struct ScExec* exec)
{
    exec->i.trader.rec = get_id(cache, SC_ENTITY_ACCNT, exec->i.trader.id_only);
    exec->i.giveup.rec = get_id(cache, SC_ENTITY_ACCNT, exec->i.giveup.id_only);
    exec->i.contr.rec = get_id(cache, SC_ENTITY_CONTR, exec->i.contr.id_only);
    if (exec->cpty.id_only)
        exec->cpty.rec = get_id(cache, SC_ENTITY_ACCNT, exec->cpty.id_only);
    else
        exec->cpty.rec = NULL;
    return exec;
}

static inline struct ScPosn*
enrich_posn(struct FigCache* cache, struct ScPosn* posn)
{
    posn->accnt.rec = get_id(cache, SC_ENTITY_ACCNT, posn->accnt.id_only);
    posn->contr.rec = get_id(cache, SC_ENTITY_CONTR, posn->contr.id_only);
    return posn;
}

static inline struct ScView*
enrich_view(struct FigCache* cache, struct ScView* view)
{
    view->contr.rec = get_id(cache, SC_ENTITY_CONTR, view->contr.id_only);
    return view;
}

static ScAccnt
get_accnt(ScClnt clnt, ScIden aid)
{
    struct ScRec* arec = get_id(&clnt->cache, SC_ENTITY_ACCNT, aid);
    assert(arec->accnt.state);
    return arec->accnt.state;
}

static void
emplace_trader_list(ScClnt clnt, struct ScSlNode* first)
{
    for (struct ScSlNode* node = first; node; node = node->next) {
        struct ScPerm* perm = enrich_perm(&clnt->cache, sc_shared_perm_entry(node));
        ScAccnt giveup = perm->giveup.rec->accnt.state;
        assert(giveup);
        // Transfer ownership.
        fig_accnt_emplace_trader(giveup, perm);
    }
}

static void
emplace_giveup_list(ScClnt clnt, struct ScSlNode* first)
{
    for (struct ScSlNode* node = first; node; node = node->next) {
        struct ScPerm* perm = enrich_perm(&clnt->cache, sc_shared_perm_entry(node));
        ScAccnt trader = perm->trader.rec->accnt.state;
        assert(trader);
        // Transfer ownership.
        fig_accnt_emplace_giveup(trader, perm);
    }
}

static void
emplace_order_list(ScClnt clnt, struct ScSlNode* first)
{
    for (struct ScSlNode* node = first; node; node = node->next) {
        struct ScOrder* order = enrich_order(&clnt->cache, sc_shared_order_entry(node));
        ScAccnt trader = order->i.trader.rec->accnt.state;
        assert(trader);
        // Transfer ownership.
        fig_accnt_emplace_order(trader, order);
    }
}

static void
emplace_exec_list(ScClnt clnt, struct ScSlNode* first)
{
    for (struct ScSlNode* node = first; node; node = node->next) {
        struct ScExec* exec = enrich_exec(&clnt->cache, sc_shared_exec_entry(node));
        assert(exec->i.state == SC_STATE_TRADE);
        ScAccnt trader = exec->i.trader.rec->accnt.state;
        assert(trader);
        // Transfer ownership.
        fig_accnt_insert_trade(trader, exec);
        sc_exec_decref(exec, clnt->pool);
    }
}

static ScBool
emplace_posn_list(ScClnt clnt, struct ScSlNode* first)
{
    ScBool nomem = SC_FALSE;
    for (struct ScSlNode* node = first; node; ) {
        struct ScPosn* posn = enrich_posn(&clnt->cache, sc_shared_posn_entry(node));
        node = node->next;
        // Transfer ownership.
        // All accounts that trader is member of are created in emplace_perms().
        ScAccnt accnt = fig_accnt_lazy(posn->accnt.rec, &clnt->ordidx, clnt->pool);
        if (sc_likely(accnt)) {
            fig_accnt_emplace_posn(accnt, posn);
        } else {
            sc_pool_free_posn(clnt->pool, posn);
            nomem = SC_TRUE;
        }
    }
    if (sc_unlikely(nomem)) {
        sc_err_set(SC_ENOMEM, "out of memory");
        return SC_FALSE;
    }
    return SC_TRUE;
}

static struct ScOrder*
create_order(ScClnt clnt, struct ScExec* exec)
{
    struct ScOrder* order = sc_pool_alloc_order(clnt->pool);
    if (!order)
        return NULL;
    sc_order_init(order);

    order->level = NULL;
    order->id = exec->order;
    __builtin_memcpy(&order->i, &exec->i, sizeof(struct ScCommon));
    order->created = exec->created;
    order->modified = exec->created;
    return order;
}

static ScBool
apply_new(ScClnt clnt, struct ScExec* exec)
{
    struct ScOrder* order = create_order(clnt, exec);
    if (!order)
        return SC_FALSE;
    ScAccnt trader = order->i.trader.rec->accnt.state;
    assert(trader);
    // Transfer ownership.
    fig_accnt_emplace_order(trader, order);
    return SC_TRUE;
}

static ScBool
apply_update(ScClnt clnt, struct ScExec* exec)
{
    ScAccnt trader = exec->i.trader.rec->accnt.state;
    assert(trader);
    struct ScRbNode* node = fig_accnt_find_order_id(trader, exec->order);
    if (!node) {
        sc_err_setf(SC_EINVAL, "no such order '%ld'", exec->order);
        return SC_FALSE;
    }

    struct ScOrder* order = sc_accnt_order_entry(node);
    order->i.state = exec->i.state;
    order->i.lots = exec->i.lots;
    order->i.resd = exec->i.resd;
    order->i.exec = exec->i.exec;
    order->i.last_ticks = exec->i.last_ticks;
    order->i.last_lots = exec->i.last_lots;
    order->modified = exec->created;

    if (exec->i.state == SC_STATE_TRADE) {
        // Transfer ownership.
        fig_accnt_insert_trade(trader, exec);
    }
    return SC_TRUE;
}

static struct ScPosn*
apply_posnup(ScClnt clnt, struct ScPosn* posn)
{
    ScAccnt accnt = fig_accnt_lazy(posn->accnt.rec, &clnt->ordidx, clnt->pool);
    if (sc_unlikely(!accnt)) {
        sc_pool_free_posn(accnt->pool, posn);
        sc_err_set(SC_ENOMEM, "out of memory");
        return NULL;
    }
    posn = fig_accnt_update_posn(accnt, posn);
    return posn;
}

static struct ScView*
apply_viewup(ScClnt clnt, struct ScView* view)
{
    const ScKey key = sc_view_key(view->contr.rec->id, view->settl_day);
    struct ScRbNode* node = sc_tree_insert(&clnt->views, key, &view->clnt_node_);
    if (node != &view->clnt_node_) {

        struct ScView* exist = sc_clnt_view_entry(node);

        // Drop if node already exists and overwrite is not set.
        if (sc_unlikely(view->created < exist->created)) {
            sc_log_warn("dropped stale view");
            sc_pool_free_view(clnt->pool, view);
            return NULL;
        }

        // Update existing position.

        assert(exist->contr.rec == view->contr.rec);
        assert(exist->settl_day == view->settl_day);

        for (size_t i = 0; i < SC_LEVEL_MAX; ++i) {
            exist->bid_ticks[i] = view->bid_ticks[i];
            exist->bid_lots[i] = view->bid_lots[i];
            exist->bid_count[i] = view->bid_count[i];
            exist->offer_ticks[i] = view->offer_ticks[i];
            exist->offer_lots[i] = view->offer_lots[i];
            exist->offer_count[i] = view->offer_count[i];
        }
        exist->created = view->created;

        sc_pool_free_view(clnt->pool, view);
        view = exist;
    }
    return view;
}

static void
apply_views(ScClnt clnt, struct ScSlNode* first, ScHandler handler)
{
    // In the following example, the snapshot should overwrite the T+0 delta, but not the T+2 delta:
    // T+0: EURUSD delta
    // T+2: USDJPY delta
    // T+1: snapshot

    for (struct ScSlNode* node = first; node; ) {
        struct ScView* view = sc_shared_view_entry(node);
        node = node->next;
        // Transfer ownership or free.
        enrich_view(&clnt->cache, view);
        view = apply_viewup(clnt, view);
        if (sc_likely(view))
            sc_handler_on_view(handler, clnt, view);
    }
    sc_handler_on_flush(handler, clnt);
}

SC_API ScBool
sc_clnt_dispatch(ScClnt clnt, ScMillis ms, ScHandler handler)
{
    assert(ms >= 0);

    if (clnt->state == FIG_CLOSED) {
        sc_err_set(SC_EBUSY, "client is closed");
        goto fail1;
    }

    ScMillis now = sc_millis();
    const ScMillis absms = now + ms;
    // At least one iteration.
    do {
        if (clnt->state == (FIG_DELTA_WAIT | FIG_CLOSE_WAIT)) {
            sc_log_debug1("close immediate");
            clnt->state = FIG_CLOSED;
            fig_clnt_log_state(clnt->state);
            sc_handler_on_close(handler, clnt);
            goto done;
        }
        const struct ScElem* elem;
        while ((elem = sc_prioq_top(&clnt->prioq))) {
            if (elem->key > now) {
                // Millis until next timeout.
                ms = sc_min(absms, elem->key) - now;
                break;
            }
            // Internal timeout.
            const ScKey key = elem->key;
            const ScIden id = elem->id;
            sc_prioq_pop(&clnt->prioq);

            if (id == FIG_HBTMR) {
                // Cannot fail due to pop.
                sc_prioq_push(&clnt->prioq, id, key + clnt->sess.hbint);
                struct ScBody body = { .req_id = 0, .sid = clnt->sess.sid,
                                        .type = SC_SESS_HEARTBT };
                if (!sc_send_body(clnt->trsock, &body, SC_FALSE))
                    goto fail1;
                // Next heartbeat may have already expired.
            } else if (id == FIG_MDTMR) {
                sc_log_debug1("market-data socket timeout");
                if (!(clnt->state & (FIG_DELTA_WAIT | FIG_CLOSE_WAIT))) {
                    fig_clnt_sess_reset(clnt);
                    sc_handler_on_reset(handler, clnt);
                }
                // Cannot fail due to pop.
                sc_prioq_push(&clnt->prioq, id, LONG_MAX);
                sc_err_setf(SC_ETIMEOUT, "market-data socket timeout");
                goto fail1;
            } else if (id == FIG_TRTMR) {
                sc_log_debug1("transaction socket timeout");
                if (!(clnt->state & (FIG_DELTA_WAIT | FIG_CLOSE_WAIT))) {
                    fig_clnt_sess_reset(clnt);
                    sc_handler_on_reset(handler, clnt);
                }
                // Cannot fail due to pop.
                sc_prioq_push(&clnt->prioq, id, LONG_MAX);
                sc_err_setf(SC_ETIMEOUT, "transaction socket timeout");
                goto fail1;
            } else if (id == clnt->close_id) {
                sc_log_debug1("close timeout");
                clnt->state = FIG_CLOSED;
                fig_clnt_log_state(clnt->state);
                sc_handler_on_close(handler, clnt);
                goto done;
            } else {
                // Assumed that these "top-half" handlers do not block.
                sc_handler_on_timeout(handler, clnt, id);
            }
        }

        // From the zmq_poll() man page:

        // For each zmq_pollitem_t item, zmq_poll() shall first clear the revents member, and then
        // indicate any requested events that have occurred by setting the bit corresponding to the
        // event condition in the revents member.

        // Note also that zmq_poll() is level-triggered.

        const int nevents = zmq_poll(clnt->items, FIG_NSOCK, ms);
        if (nevents < 0) {
            sc_err_setf(SC_EIO, "zmq_poll() failed: %s", zmq_strerror(zmq_errno()));
            goto fail1;
        }
        // Current time after slow operation.
        now = sc_millis();
        if (nevents == 0)
            continue;

        struct ScBody body;
        if ((clnt->items[FIG_TRSOCK].revents & ZMQ_POLLIN)) {

            if (!sc_recv_body(clnt->trsock, clnt->pool, &body))
                goto fail1;

            if (body.sid < clnt->sess.sid) {
                sc_log_warn("ignoring message from session %d", body.sid);
                goto mdsock;
            }

            if (body.req_id > 0)
                sc_prioq_remove(&clnt->prioq, body.req_id);

            sc_prioq_replace(&clnt->prioq, FIG_TRTMR, now + FIG_TRTMOUT);

            switch (body.type) {
            case SC_SESS_OPEN:
                sc_log_debug1("open message");
                clnt->sess.hbint = body.sess_open.hbint;
                clnt->state &= ~FIG_OPEN_WAIT;
                clnt->state |= (FIG_REC_WAIT | FIG_SNAP_WAIT);
                fig_clnt_log_state(clnt->state);
                if (!sc_prioq_push(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint)
                    || !sc_prioq_push(&clnt->prioq, FIG_TRTMR, now + FIG_TRTMOUT))
                    goto fail1;
                break;
            case SC_SESS_CLOSE:
                sc_log_debug1("close message");
                // Set closed status before handler is called.
                const unsigned prev = clnt->state;
                clnt->state = FIG_CLOSED;
                fig_clnt_log_state(clnt->state);
                sc_handler_on_close(handler, clnt);
                // If we were originally expecting a close, then we're done.
                if ((prev & FIG_CLOSE_WAIT))
                    goto done;
                // Reset if state remains closed after handler has been called.
                if (clnt->state == FIG_CLOSED) {
                    fig_clnt_sess_reset(clnt);
                    sc_handler_on_reset(handler, clnt);
                }
                break;
            case SC_SESS_LOGON:
                sc_log_debug1("logon message");
                sc_sess_logon(&clnt->sess, get_accnt(clnt, body.sess_logon.aid));
                sc_handler_on_logon(handler, clnt, body.req_id, body.sess_logon.aid);
                break;
            case SC_SESS_LOGOFF:
                sc_log_debug1("logoff message");
                sc_handler_on_logoff(handler, clnt, body.req_id, body.sess_logoff.aid);
                {
                    ScAccnt accnt = get_accnt(clnt, body.sess_logoff.aid);
                    sc_sess_logoff_and_reset(&clnt->sess, accnt);
                }
                break;
            case SC_SESS_HEARTBT:
                sc_log_debug3("heartbeat");
                break;
            case SC_STATUS_REP:
                sc_log_debug1("status message");
                sc_handler_on_status(handler, clnt, body.req_id, body.status_rep.num,
                                      body.status_rep.msg);
                break;
            case SC_ACCNT_LIST_REP:
                sc_log_debug1("accnt-list message");
                clnt->state &= ~FIG_ACCNT_WAIT;
                fig_clnt_log_state(clnt->state);
                fig_cache_emplace_rec_list(&clnt->cache, SC_ENTITY_ACCNT,
                                           body.entity_list_rep.first, body.entity_list_rep.count_);
                if (clnt->state == FIG_READY)
                    sc_handler_on_ready(handler, clnt);
                break;
            case SC_CONTR_LIST_REP:
                sc_log_debug1("contr-list message");
                clnt->state &= ~FIG_CONTR_WAIT;
                fig_clnt_log_state(clnt->state);
                fig_cache_emplace_rec_list(&clnt->cache, SC_ENTITY_CONTR,
                                           body.entity_list_rep.first, body.entity_list_rep.count_);
                if (clnt->state == FIG_READY)
                    sc_handler_on_ready(handler, clnt);
                break;
            case SC_TRADER_LIST_REP:
                sc_log_debug1("trader-list message");
                emplace_trader_list(clnt, body.entity_list_rep.first);
                break;
            case SC_GIVEUP_LIST_REP:
                sc_log_debug1("giveup-list message");
                emplace_giveup_list(clnt, body.entity_list_rep.first);
                break;
            case SC_ORDER_LIST_REP:
                sc_log_debug1("order-list message");
                emplace_order_list(clnt, body.entity_list_rep.first);
                break;
            case SC_EXEC_LIST_REP:
                sc_log_debug1("exec-list message");
                emplace_exec_list(clnt, body.entity_list_rep.first);
                break;
            case SC_POSN_LIST_REP:
                sc_log_debug1("posn-list message");
                // This function can fail is there is no memory available for a lazily created
                // account.
                if (sc_unlikely(!emplace_posn_list(clnt, body.entity_list_rep.first))) {
                    fig_clnt_sess_reset(clnt);
                    goto fail1;
                }
                break;
            case SC_VIEW_LIST_REP:
                sc_log_debug1("view-list message");
                clnt->state &= ~FIG_SNAP_WAIT;
                fig_clnt_log_state(clnt->state);
                apply_views(clnt, body.view_list_rep.first, handler);
                if (clnt->state == FIG_READY)
                    sc_handler_on_ready(handler, clnt);
                break;
            case SC_EXEC_REP:
                sc_log_debug1("exec message");
                enrich_exec(&clnt->cache, body.exec_rep.exec);
                switch (body.exec_rep.exec->i.state) {
                case SC_STATE_NEW:
                    apply_new(clnt, body.exec_rep.exec);
                    break;
                case SC_STATE_REVISE:
                case SC_STATE_CANCEL:
                case SC_STATE_TRADE:
                    apply_update(clnt, body.exec_rep.exec);
                    break;
                }
                sc_handler_on_exec(handler, clnt, body.req_id, body.exec_rep.exec);
                if (sc_exec_done(body.exec_rep.exec)) {
                    struct ScExec* exec = body.exec_rep.exec;
                    ScAccnt trader = exec->i.trader.rec->accnt.state;
                    assert(trader);
                    struct ScOrder* order = fig_accnt_release_order_id(trader, exec->order);
                    if (order)
                        sc_pool_free_order(clnt->pool, order);
                }
                sc_exec_decref(body.exec_rep.exec, clnt->pool);
                break;
            case SC_POSN_REP:
                sc_log_debug1("posn message");
                enrich_posn(&clnt->cache, body.posn_rep.posn);
                // This function can fail is there is no memory available for a lazily created
                // account.
                body.posn_rep.posn = apply_posnup(clnt, body.posn_rep.posn);
                if (sc_unlikely(!body.posn_rep.posn))
                    goto fail1;
                sc_handler_on_posn(handler, clnt, body.posn_rep.posn);
                break;
            default:
                sc_err_setf(SC_EIO, "unknown body-type '%d'", body.type);
                goto fail1;
            }
        }

    mdsock:
        if ((clnt->items[FIG_MDSOCK].revents & ZMQ_POLLIN)) {

            if (!sc_recv_body(clnt->mdsock, clnt->pool, &body))
                goto fail1;

            sc_prioq_replace(&clnt->prioq, FIG_MDTMR, now + FIG_MDTMOUT);

            switch (body.type) {
            case SC_VIEW_LIST_REP:
                if (sc_unlikely(clnt->state & FIG_DELTA_WAIT)) {
                    if (clnt->state == (FIG_DELTA_WAIT | FIG_CLOSE_WAIT)) {
                        sc_log_debug1("close immediate");
                        clnt->state = FIG_CLOSED;
                        fig_clnt_log_state(clnt->state);
                        sc_handler_on_close(handler, clnt);
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
                sc_err_setf(SC_EIO, "unknown body-type '%d'", body.type);
                goto fail1;
            }
        }

        if ((clnt->items[FIG_ASOCK].revents & ZMQ_POLLIN)) {

            void* val;
            if (!fig_async_recv(clnt->asock, &val))
                goto fail1;

            if (sc_unlikely(val == (void*)~0)) {
                if (fig_clnt_sess_close(clnt, now) < 0)
                    goto fail1;
                val = NULL;
            } else
                val = sc_handler_on_async(handler, clnt, val);

            if (!fig_async_send(clnt->asock, val))
                goto fail1;
        }
    } while (now < absms);
 done:
    return SC_TRUE;
 fail1:
    return SC_FALSE;
}
