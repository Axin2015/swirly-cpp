/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/fig/serv.h>

#include "accnt.h"
#include "cache.h"
#include "match.h"
#include "ordidx.h"
#include "sessidx.h"

#include <sc/fig/book.h>

#include <sc/ash/bank.h>
#include <sc/ash/err.h>
#include <sc/ash/queue.h>
#include <sc/ash/util.h>

#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

struct FigServ {
    struct ScBank bank;
    ScJourn journ;
    ScPool pool;
    struct FigCache cache;
    struct FigOrdIdx ordidx;
    struct FigSessIdx sessidx;
    struct ScTree books;

    struct ScQueue execs;
    struct ScTree posnups;
    struct ScTree bookups;
};

static inline struct ScBook*
serv_book_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScBook, serv_node_, node);
}

static void
term_state(struct ScRec* rec)
{
    switch (rec->type) {
    case SC_ENTITY_ACCNT:
        fig_accnt_term(rec);
        break;
    }
}

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
enrich_trade(struct FigCache* cache, struct ScExec* exec)
{
    exec->i.trader.rec = get_id(cache, SC_ENTITY_ACCNT, exec->i.trader.id_only);
    exec->i.giveup.rec = get_id(cache, SC_ENTITY_ACCNT, exec->i.giveup.id_only);
    exec->i.contr.rec = get_id(cache, SC_ENTITY_CONTR, exec->i.contr.id_only);
    exec->cpty.rec = get_id(cache, SC_ENTITY_ACCNT, exec->cpty.id_only);
    return exec;
}

static inline struct ScPosn*
enrich_posn(struct FigCache* cache, struct ScPosn* posn)
{
    posn->accnt.rec = get_id(cache, SC_ENTITY_ACCNT, posn->accnt.id_only);
    posn->contr.rec = get_id(cache, SC_ENTITY_CONTR, posn->contr.id_only);
    return posn;
}

static inline void
insert_posnup(struct ScTree* posnups, struct ScPosn* posn)
{
    const ScKey key = sc_posn_key(posn->accnt.rec->id, posn->contr.rec->id, posn->settl_day);
    sc_tree_insert(posnups, key, &posn->update_node_);
}

static inline struct ScBook*
get_book(ScServ serv, ScKey key)
{
	struct ScRbNode* node = sc_tree_find(&serv->books, key);
    return node ? serv_book_entry(node) : NULL;
}

static struct ScBook*
lazy_book(ScServ serv, struct ScRec* crec, ScJd settl_day)
{
    assert(crec);
    assert(crec->type == SC_ENTITY_CONTR);

    const ScKey key = sc_book_key(crec->id, settl_day);
    struct ScBook* book;
	struct ScRbNode* node = sc_tree_pfind(&serv->books, key);
    if (!node || node->key != key) {
        book = sc_pool_alloc_book(serv->pool);
        if (sc_unlikely(!book)) {
            sc_err_set(SC_ENOMEM, "out of memory");
            return NULL;
        }
        sc_book_init(book, crec, settl_day, serv->pool);
        struct ScRbNode* parent = node;
        sc_tree_pinsert(&serv->books, key, &book->serv_node_, parent);
    } else
        book = serv_book_entry(node);
    return book;
}

static void
free_books(struct ScTree* books, ScPool pool)
{
    assert(books);
    struct ScRbNode* node;
    while ((node = books->root)) {
        struct ScBook* book = serv_book_entry(node);
        sc_tree_remove(books, node);
        sc_book_term(book);
        sc_pool_free_book(pool, book);
    }
}

static void
free_match_list(struct ScSlNode* first, ScPool pool)
{
    struct ScSlNode* node = first;
    while (node) {
        struct ScMatch* match = sc_trans_match_entry(node);
        node = node->next;
        // Not committed so match object still owns the trades.
        sc_exec_decref(match->taker_exec, pool);
        sc_exec_decref(match->maker_exec, pool);
        sc_pool_free_match(pool, match);
    }
}

static ScBool
emplace_rec_list(ScServ serv, ScModel model, int type)
{
    struct ScSlNode* node;
    ssize_t size = sc_model_read_entity(model, type, serv->pool, &node);
    if (size < 0)
        return SC_FALSE;

    fig_cache_emplace_rec_list(&serv->cache, type, node, size);
    return SC_TRUE;
}

static ScBool
emplace_perms(ScServ serv, ScModel model)
{
    struct ScSlNode* node;
    if (sc_model_read_entity(model, SC_ENTITY_PERM, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct ScPerm* uperm = enrich_perm(&serv->cache, sc_shared_perm_entry(node));

        // Clone member for giveup.
        struct ScPerm* gperm = sc_pool_alloc_perm(serv->pool);
        if (!gperm)
            goto fail2;
        sc_perm_init(gperm);
        gperm->trader.rec = uperm->trader.rec;
        gperm->giveup.rec = uperm->giveup.rec;

        struct FigAccnt* trader = fig_accnt_lazy(uperm->trader.rec, &serv->ordidx, serv->pool);
        if (sc_unlikely(!trader)) {
            sc_pool_free_perm(serv->pool, gperm);
            goto fail2;
        }

        struct FigAccnt* giveup = fig_accnt_lazy(uperm->giveup.rec, &serv->ordidx, serv->pool);
        if (sc_unlikely(!giveup)) {
            sc_pool_free_perm(serv->pool, gperm);
            goto fail2;
        }

        // Transfer ownership.
        fig_accnt_emplace_trader(giveup, uperm);
        fig_accnt_emplace_giveup(trader, gperm);
    }
    return SC_TRUE;
 fail2:
    // Free tail.
    do {
        struct ScPerm* perm = sc_shared_perm_entry(node);
        node = node->next;
        sc_pool_free_perm(serv->pool, perm);
    } while (node);
 fail1:
    return SC_FALSE;
}

static ScBool
emplace_orders(ScServ serv, ScModel model)
{
    struct ScSlNode* node;
    if (sc_model_read_entity(model, SC_ENTITY_ORDER, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct ScOrder* order = enrich_order(&serv->cache, sc_shared_order_entry(node));
        struct ScBook* book;
        if (!sc_order_done(order)) {

            book = lazy_book(serv, order->i.contr.rec, order->i.settl_day);
            if (sc_unlikely(!book))
                goto fail2;

            if (sc_unlikely(!sc_book_insert(book, order)))
                goto fail2;
        } else
            book = NULL;

        struct FigAccnt* trader = fig_accnt_lazy(order->i.trader.rec, &serv->ordidx, serv->pool);
        if (sc_unlikely(!trader)) {
            if (book)
                sc_book_remove(book, order);
            goto fail2;
        }

        // Transfer ownership.
        fig_accnt_emplace_order(trader, order);
    }
    return SC_TRUE;
 fail2:
    // Free tail.
    do {
        struct ScOrder* order = sc_shared_order_entry(node);
        node = node->next;
        sc_pool_free_order(serv->pool, order);
    } while (node);
 fail1:
    return SC_FALSE;
}

static ScBool
emplace_trades(ScServ serv, ScModel model)
{
    struct ScSlNode* node;
    if (sc_model_read_entity(model, SC_ENTITY_EXEC, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct ScExec* exec = enrich_trade(&serv->cache, sc_shared_exec_entry(node));
        struct FigAccnt* trader = fig_accnt_lazy(exec->i.trader.rec, &serv->ordidx, serv->pool);
        if (sc_unlikely(!trader))
            goto fail2;

        // Transfer ownership.
        fig_accnt_insert_trade(trader, exec);
        sc_exec_decref(exec, serv->pool);
    }
    return SC_TRUE;
 fail2:
    // Free tail.
    do {
        struct ScExec* exec = sc_shared_exec_entry(node);
        node = node->next;
        sc_exec_decref(exec, serv->pool);
    } while (node);
 fail1:
    return SC_FALSE;
}

static ScBool
emplace_posns(ScServ serv, ScModel model)
{
    struct ScSlNode* node;
    if (sc_model_read_entity(model, SC_ENTITY_POSN, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct ScPosn* posn = enrich_posn(&serv->cache, sc_shared_posn_entry(node));
        struct FigAccnt* accnt = fig_accnt_lazy(posn->accnt.rec, &serv->ordidx, serv->pool);
        if (sc_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        fig_accnt_emplace_posn(accnt, posn);
    }
    return SC_TRUE;
 fail2:
    // Free tail.
    do {
        struct ScPosn* posn = sc_shared_posn_entry(node);
        node = node->next;
        sc_pool_free_posn(serv->pool, posn);
    } while (node);
 fail1:
    return SC_FALSE;
}

static struct ScExec*
create_exec(ScServ serv, struct ScOrder* order, ScMillis now)
{
    struct ScExec* exec = sc_pool_alloc_exec(serv->pool);
    if (!exec)
        return NULL;
    sc_exec_init(exec);

    exec->id = sc_bank_add_fetch(&serv->bank, SC_REG_EXEC, 1L);
    exec->order = order->id;
    __builtin_memcpy(&exec->i, &order->i, sizeof(struct ScCommon));
    exec->match = 0;
    exec->role = 0;
    exec->cpty.id_only = 0;
    exec->created = now;
    return exec;
}

static inline void
apply_posn(struct ScPosn* posn, const struct ScExec* exec)
{
    const double licks = exec->i.last_lots * exec->i.last_ticks;
    if (exec->i.action == SC_ACTION_BUY) {
        posn->buy_licks += licks;
        posn->buy_lots += exec->i.last_lots;
    } else {
        assert(exec->i.action == SC_ACTION_SELL);
        posn->sell_licks += licks;
        posn->sell_lots += exec->i.last_lots;
    }
}

// Assumes that maker lots have not been reduced since matching took place.

static void
commit_trans(ScServ serv, struct FigAccnt* taker, struct ScBook* book,
             struct ScTrans* trans, ScMillis now)
{
    while (!sc_queue_empty(&trans->matches)) {

        struct ScSlNode* node = sc_queue_pop(&trans->matches);
        struct ScMatch* match = sc_trans_match_entry(node);
        struct ScOrder* maker_order = match->maker_order;

        // Reduce maker.
        sc_book_take(book, maker_order, match->lots, now);
        insert_posnup(&serv->posnups, match->maker_posn);

        // Must succeed because maker order exists.
        struct FigAccnt* maker = fig_accnt_lazy(maker_order->i.trader.rec, &serv->ordidx,
                                                  serv->pool);
        assert(maker);

        // Maker updated first because this is consistent with last-look semantics.

        // Update maker.
        fig_accnt_insert_trade(maker, match->maker_exec);
        apply_posn(match->maker_posn, match->maker_exec);

        // Update taker.
        fig_accnt_insert_trade(taker, match->taker_exec);
        apply_posn(trans->taker_posn, match->taker_exec);

        // Exec ownership is effectively transferred to serv->execs outside of loop, so no need to
        // decrement Exec reference count here.
        sc_pool_free_match(serv->pool, match);
    }

    sc_queue_join(&serv->execs, &trans->execs);
    if (trans->taker_posn)
        insert_posnup(&serv->posnups, trans->taker_posn);
}

SC_API ScServ
sc_serv_create(const char* bank, ScJourn journ, ScPool pool)
{
    // 1.
    ScServ serv = malloc(sizeof(struct FigServ));
    if (sc_unlikely(!serv)) {
        sc_err_set(SC_ENOMEM, "out of memory");
        goto fail1;
    }

    // 2.
    if (!sc_bank_init(&serv->bank, bank, SC_REG_LEN))
        goto fail2;

    serv->journ = journ;
    serv->pool = pool;
    // 3.
    fig_cache_init(&serv->cache, term_state, pool);
    fig_ordidx_init(&serv->ordidx);
    // 4.
    fig_sessidx_init(&serv->sessidx, pool);
    // 5.
    sc_tree_init(&serv->books);

    sc_queue_init(&serv->execs);
    sc_tree_init(&serv->posnups);
    sc_tree_init(&serv->bookups);

    return serv;
 fail2:
    free(serv);
 fail1:
    return NULL;
}

SC_API void
sc_serv_destroy(ScServ serv)
{
    if (serv) {
        // Ensure that executions are freed.
        sc_serv_clear(serv);
        // 5.
        free_books(&serv->books, serv->pool);
        // 4.
        fig_sessidx_term(&serv->sessidx);
        // 3.
        fig_cache_term(&serv->cache);
        // 2.
        sc_bank_term(&serv->bank);
        // 1.
        free(serv);
    }
}

// Cache

SC_API ScBool
sc_serv_load(ScServ serv, ScModel model)
{
    return emplace_rec_list(serv, model, SC_ENTITY_ACCNT)
        && emplace_rec_list(serv, model, SC_ENTITY_CONTR)
        && emplace_perms(serv, model)
        && emplace_orders(serv, model)
        && emplace_trades(serv, model)
        && emplace_posns(serv, model);
}

SC_API struct ScSlNode*
sc_serv_find_rec_id(ScServ serv, int type, ScIden id)
{
    return fig_cache_find_rec_id(&serv->cache, type, id);
}

SC_API struct ScSlNode*
sc_serv_find_rec_mnem(ScServ serv, int type, const char* mnem)
{
    return fig_cache_find_rec_mnem(&serv->cache, type, mnem);
}

SC_API struct ScSlNode*
sc_serv_first_rec(ScServ serv, int type, size_t* size)
{
    return fig_cache_first_rec(&serv->cache, type, size);
}

SC_API ScBool
sc_serv_empty_rec(ScServ serv, int type)
{
    return fig_cache_empty_rec(&serv->cache, type);
}

// Pool

SC_API ScAccnt
sc_serv_accnt(ScServ serv, struct ScRec* arec)
{
    return fig_accnt_lazy(arec, &serv->ordidx, serv->pool);
}

SC_API struct ScBook*
sc_serv_book(ScServ serv, struct ScRec* crec, ScJd settl_day)
{
    return lazy_book(serv, crec, settl_day);
}

SC_API struct ScSess*
sc_serv_sess(ScServ serv, const ScUuid uuid)
{
    return fig_sessidx_lazy(&serv->sessidx, uuid);
}

// Exec

SC_API struct ScOrder*
sc_serv_place(ScServ serv, ScAccnt trader, ScAccnt giveup, struct ScBook* book,
               const char* ref, int action, ScTicks ticks, ScLots lots, ScLots min_lots)
{
    if (!fig_accnt_logged_on(trader)) {
        sc_err_setf(SC_EEXIST, "not logged-on '%.16s'", trader->rec->mnem);
        goto fail1;
    }

    if (lots == 0 || lots < min_lots) {
        sc_err_setf(SC_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    const ScMillis now = sc_millis();
    struct ScOrder* new_order = sc_pool_alloc_order(serv->pool);
    if (!new_order)
        goto fail1;
    sc_order_init(new_order);

    new_order->level = NULL;
    new_order->id = sc_bank_add_fetch(&serv->bank, SC_REG_ORDER, 1L);
    new_order->i.trader.rec = fig_accnt_rec(trader);
    new_order->i.giveup.rec = fig_accnt_rec(giveup);
    new_order->i.contr.rec = book->crec;
    new_order->i.settl_day = book->settl_day;
    if (ref)
        strncpy(new_order->i.ref, ref, SC_REF_MAX);
    else
        new_order->i.ref[0] = '\0';
    new_order->i.state = SC_STATE_NEW;
    new_order->i.action = action;
    new_order->i.ticks = ticks;
    new_order->i.lots = lots;
    new_order->i.resd = lots;
    new_order->i.exec = 0;
    new_order->i.last_ticks = 0;
    new_order->i.last_lots = 0;
    new_order->i.min_lots = min_lots;
    new_order->created = now;
    new_order->modified = now;

    struct ScExec* new_exec = create_exec(serv, new_order, now);
    if (!new_exec)
        goto fail2;

    struct ScTrans trans;
    sc_trans_init(&trans);

    sc_queue_insert_front(&trans.execs, &new_exec->shared_node_);

    // Order fields are updated on match.
    if (!fig_match_orders(book, new_order, &serv->bank, &serv->ordidx, serv->pool, &trans))
        goto fail3;

    // Place incomplete order in book.
    if (!sc_order_done(new_order) && !sc_book_insert(book, new_order))
        goto fail4;

    // TODO: IOC orders would need an additional revision for the unsolicited cancellation of any
    // unfilled quantity.

    if (!sc_journ_insert_exec_list(serv->journ, trans.execs.first, SC_TRUE))
        goto fail5;

    // Final commit phase cannot fail.
    fig_accnt_emplace_order(trader, new_order);
    const ScKey key = sc_book_key(book->crec->id, book->settl_day);
    sc_tree_insert(&serv->bookups, key, &book->update_node_);
    // Commit trans to cycle and free matches.
    commit_trans(serv, trader, book, &trans, now);
    return new_order;
 fail5:
    if (!sc_order_done(new_order))
        sc_book_remove(book, new_order);
 fail4:
    free_match_list(sc_queue_first(&trans.matches), serv->pool);
 fail3:
    sc_exec_decref(new_exec, serv->pool);
 fail2:
    sc_pool_free_order(serv->pool, new_order);
 fail1:
    return NULL;
}

SC_API struct ScOrder*
sc_serv_revise_id(ScServ serv, ScAccnt trader, ScIden id, ScLots lots)
{
    if (!fig_accnt_logged_on(trader)) {
        sc_err_setf(SC_EEXIST, "not logged-on '%.16s'", trader->rec->mnem);
        goto fail1;
    }

    struct ScRbNode* node = fig_accnt_find_order_id(trader, id);
    if (!node) {
        sc_err_setf(SC_EINVAL, "no such order '%ld'", id);
        goto fail1;
    }

    struct ScOrder* order = sc_accnt_order_entry(node);
    if (sc_order_done(order)) {
        sc_err_setf(SC_EINVAL, "order complete '%ld'", id);
        goto fail1;
    }

    // Revised lots must not be:
    // 1. less than min lots;
    // 2. less than executed lots;
    // 3. greater than original lots.

    if (lots == 0 || lots < order->i.min_lots || lots < order->i.exec || lots > order->i.lots) {
        sc_err_setf(SC_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    const ScMillis now = sc_millis();
    struct ScExec* exec = create_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Revise.
    exec->i.state = SC_STATE_REVISE;
    const ScLots delta = exec->i.lots - lots;
    assert(delta >= 0);
    exec->i.lots = lots;
    exec->i.resd -= delta;

    if (!sc_journ_insert_exec(serv->journ, exec, SC_TRUE))
        goto fail2;

    // Final commit phase cannot fail.
    const ScKey key = sc_book_key(order->i.contr.rec->id, order->i.settl_day);
    struct ScBook* book = get_book(serv, key);
    assert(book);
    sc_book_revise(book, order, lots, now);
    sc_tree_insert(&serv->bookups, key, &book->update_node_);
    sc_queue_insert_back(&serv->execs, &exec->shared_node_);
    return order;
 fail2:
    sc_exec_decref(exec, serv->pool);
 fail1:
    return NULL;
}

SC_API struct ScOrder*
sc_serv_revise_ref(ScServ serv, ScAccnt trader, const char* ref, ScLots lots)
{
    if (!fig_accnt_logged_on(trader)) {
        sc_err_setf(SC_EEXIST, "not logged-on '%.16s'", trader->rec->mnem);
        goto fail1;
    }

    struct ScOrder* order = fig_accnt_find_order_ref(trader, ref);
    if (!order) {
        sc_err_setf(SC_EINVAL, "no such order '%.64s'", ref);
        goto fail1;
    }

    if (sc_order_done(order)) {
        sc_err_setf(SC_EINVAL, "order complete '%.64s'", ref);
        goto fail1;
    }

    // Revised lots must not be:
    // 1. less than min lots;
    // 2. less than executed lots;
    // 3. greater than original lots.

    if (lots == 0 || lots < order->i.min_lots || lots < order->i.exec || lots > order->i.lots) {
        sc_err_setf(SC_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    const ScMillis now = sc_millis();
    struct ScExec* exec = create_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Revise.
    exec->i.state = SC_STATE_REVISE;
    const ScLots delta = exec->i.lots - lots;
    assert(delta >= 0);
    exec->i.lots = lots;
    exec->i.resd -= delta;

    if (!sc_journ_insert_exec(serv->journ, exec, SC_TRUE))
        goto fail2;

    // Final commit phase cannot fail.
    const ScKey key = sc_book_key(order->i.contr.rec->id, order->i.settl_day);
    struct ScBook* book = get_book(serv, key);
    assert(book);
    sc_book_revise(book, order, lots, now);
    sc_tree_insert(&serv->bookups, key, &book->update_node_);
    sc_queue_insert_back(&serv->execs, &exec->shared_node_);
    return order;
 fail2:
    sc_exec_decref(exec, serv->pool);
 fail1:
    return NULL;
}

SC_API struct ScOrder*
sc_serv_cancel_id(ScServ serv, ScAccnt trader, ScIden id)
{
    if (!fig_accnt_logged_on(trader)) {
        sc_err_setf(SC_EEXIST, "not logged-on '%.16s'", trader->rec->mnem);
        goto fail1;
    }

    struct ScRbNode* node = fig_accnt_find_order_id(trader, id);
    if (!node) {
        sc_err_setf(SC_EINVAL, "no such order '%ld'", id);
        goto fail1;
    }

    struct ScOrder* order = sc_accnt_order_entry(node);
    if (sc_order_done(order)) {
        sc_err_setf(SC_EINVAL, "order complete '%ld'", id);
        goto fail1;
    }

    const ScMillis now = sc_millis();
    struct ScExec* exec = create_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Cancel.
    exec->i.state = SC_STATE_CANCEL;
    exec->i.resd = 0;

    if (!sc_journ_insert_exec(serv->journ, exec, SC_TRUE))
        goto fail2;

    // Final commit phase cannot fail.
    const ScKey key = sc_book_key(order->i.contr.rec->id, order->i.settl_day);
    struct ScBook* book = get_book(serv, key);
    assert(book);
    sc_book_cancel(book, order, now);
    sc_tree_insert(&serv->bookups, key, &book->update_node_);
    sc_queue_insert_back(&serv->execs, &exec->shared_node_);
    return order;
 fail2:
    sc_exec_decref(exec, serv->pool);
 fail1:
    return NULL;
}

SC_API struct ScOrder*
sc_serv_cancel_ref(ScServ serv, ScAccnt trader, const char* ref)
{
    if (!fig_accnt_logged_on(trader)) {
        sc_err_setf(SC_EEXIST, "not logged-on '%.16s'", trader->rec->mnem);
        goto fail1;
    }

    struct ScOrder* order = fig_accnt_find_order_ref(trader, ref);
    if (!order) {
        sc_err_setf(SC_EINVAL, "no such order '%.64s'", ref);
        goto fail1;
    }

    if (sc_order_done(order)) {
        sc_err_setf(SC_EINVAL, "order complete '%.64s'", ref);
        goto fail1;
    }

    const ScMillis now = sc_millis();
    struct ScExec* exec = create_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Cancel.
    exec->i.state = SC_STATE_CANCEL;
    exec->i.resd = 0;

    if (!sc_journ_insert_exec(serv->journ, exec, SC_TRUE))
        goto fail2;

    // Final commit phase cannot fail.
    const ScKey key = sc_book_key(order->i.contr.rec->id, order->i.settl_day);
    struct ScBook* book = get_book(serv, key);
    assert(book);
    sc_book_cancel(book, order, now);
    sc_tree_insert(&serv->bookups, key, &book->update_node_);
    sc_queue_insert_back(&serv->execs, &exec->shared_node_);
    return order;
 fail2:
    sc_exec_decref(exec, serv->pool);
 fail1:
    return NULL;
}

SC_API ScBool
sc_serv_ack_trade(ScServ serv, ScAccnt trader, ScIden id)
{
    if (!fig_accnt_logged_on(trader)) {
        sc_err_setf(SC_EEXIST, "not logged-on '%.16s'", trader->rec->mnem);
        goto fail1;
    }

    struct ScRbNode* node = fig_accnt_find_trade_id(trader, id);
    if (!node) {
        sc_err_setf(SC_EINVAL, "no such trade '%ld'", id);
        goto fail1;
    }

    const ScMillis now = sc_millis();
    if (!sc_journ_update_exec(serv->journ, node->key, now))
        goto fail1;

    // No need to update timestamps on trade because it is immediately freed.

    struct ScExec* exec = sc_accnt_trade_entry(node);
    fig_accnt_remove_trade(trader, exec);
    return SC_TRUE;
 fail1:
    return SC_FALSE;
}

SC_API void
sc_serv_clear_md(ScServ serv)
{
    sc_tree_init(&serv->bookups);
}

SC_API void
sc_serv_clear_tr(ScServ serv)
{
    struct ScSlNode* node = serv->execs.first;
    while (node) {
        struct ScExec* exec = sc_serv_exec_entry(node);
        node = node->next;
        // Free completed orders.
        if (sc_exec_done(exec)) {
            ScAccnt trader = exec->i.trader.rec->accnt.state;
            assert(trader);
            struct ScOrder* order = fig_accnt_release_order_id(trader, exec->order);
            if (order)
                sc_pool_free_order(serv->pool, order);
        }
        sc_exec_decref(exec, serv->pool);
    }
    sc_queue_init(&serv->execs);
    sc_tree_init(&serv->posnups);
}

SC_API struct ScSlNode*
sc_serv_first_exec(ScServ serv)
{
    return sc_queue_first(&serv->execs);
}

SC_API ScBool
sc_serv_empty_exec(ScServ serv)
{
    return sc_queue_empty(&serv->execs);
}

SC_API struct ScRbNode*
sc_serv_first_posnup(ScServ serv)
{
    return sc_tree_first(&serv->posnups);
}

SC_API struct ScRbNode*
sc_serv_last_posnup(ScServ serv)
{
    return sc_tree_last(&serv->posnups);
}

SC_API ScBool
sc_serv_empty_posnup(ScServ serv)
{
    return sc_tree_empty(&serv->posnups);
}

// Book

SC_API struct ScRbNode*
sc_serv_find_book(ScServ serv, ScIden cid, ScJd settl_day)
{
    const ScKey key = sc_book_key(cid, settl_day);
	return sc_tree_find(&serv->books, key);
}

SC_API struct ScRbNode*
sc_serv_first_book(ScServ serv)
{
    return sc_tree_first(&serv->books);
}

SC_API struct ScRbNode*
sc_serv_last_book(ScServ serv)
{
    return sc_tree_last(&serv->books);
}

SC_API ScBool
sc_serv_empty_book(ScServ serv)
{
    return sc_tree_empty(&serv->books);
}

SC_API struct ScRbNode*
sc_serv_first_bookup(ScServ serv)
{
    return sc_tree_first(&serv->bookups);
}

SC_API struct ScRbNode*
sc_serv_last_bookup(ScServ serv)
{
    return sc_tree_last(&serv->bookups);
}

SC_API ScBool
sc_serv_empty_bookup(ScServ serv)
{
    return sc_tree_empty(&serv->bookups);
}
