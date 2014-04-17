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
#include <dbr/serv.h>

#include "accnt.h"
#include "cache.h"
#include "match.h"
#include "ordidx.h"
#include "sessidx.h"

#include <dbr/bank.h>
#include <dbr/book.h>
#include <dbr/err.h>
#include <dbr/queue.h>
#include <dbr/util.h>

#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

struct FigServ {
    struct DbrBank bank;
    DbrJourn journ;
    DbrPool pool;
    struct FigCache cache;
    struct FigOrdIdx ordidx;
    struct FigSessIdx sessidx;
    struct DbrTree books;

    struct DbrQueue execs;
    struct DbrTree posnups;
    struct DbrTree bookups;
};

static inline struct DbrBook*
serv_book_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrBook, serv_node_, node);
}

static void
term_state(struct DbrRec* rec)
{
    switch (rec->type) {
    case DBR_ENTITY_ACCNT:
        fig_accnt_term(rec);
        break;
    }
}

static inline struct DbrRec*
get_id(struct FigCache* cache, int type, DbrIden id)
{
    struct DbrSlNode* node = fig_cache_find_rec_id(cache, type, id);
    assert(node != FIG_CACHE_END_REC);
    return dbr_shared_rec_entry(node);
}

static inline struct DbrMemb*
enrich_memb(struct FigCache* cache, struct DbrMemb* memb)
{
    memb->user.rec = get_id(cache, DBR_ENTITY_ACCNT, memb->user.id_only);
    memb->group.rec = get_id(cache, DBR_ENTITY_ACCNT, memb->group.id_only);
    return memb;
}

static inline struct DbrOrder*
enrich_order(struct FigCache* cache, struct DbrOrder* order)
{
    order->c.user.rec = get_id(cache, DBR_ENTITY_ACCNT, order->c.user.id_only);
    order->c.group.rec = get_id(cache, DBR_ENTITY_ACCNT, order->c.group.id_only);
    order->c.contr.rec = get_id(cache, DBR_ENTITY_CONTR, order->c.contr.id_only);
    return order;
}

static inline struct DbrExec*
enrich_trade(struct FigCache* cache, struct DbrExec* exec)
{
    exec->c.user.rec = get_id(cache, DBR_ENTITY_ACCNT, exec->c.user.id_only);
    exec->c.group.rec = get_id(cache, DBR_ENTITY_ACCNT, exec->c.group.id_only);
    exec->c.contr.rec = get_id(cache, DBR_ENTITY_CONTR, exec->c.contr.id_only);
    exec->cpty.rec = get_id(cache, DBR_ENTITY_ACCNT, exec->cpty.id_only);
    return exec;
}

static inline struct DbrPosn*
enrich_posn(struct FigCache* cache, struct DbrPosn* posn)
{
    posn->accnt.rec = get_id(cache, DBR_ENTITY_ACCNT, posn->accnt.id_only);
    posn->contr.rec = get_id(cache, DBR_ENTITY_CONTR, posn->contr.id_only);
    return posn;
}

static inline void
insert_posnup(struct DbrTree* posnups, struct DbrPosn* posn)
{
    dbr_tree_insert(posnups, (DbrKey)posn, &posn->update_node_);
}

static inline void
insert_bookup(struct DbrTree* bookups, struct DbrBook* book)
{
    dbr_tree_insert(bookups, (DbrKey)book, &book->update_node_);
}

static inline struct DbrBook*
get_book(DbrServ serv, DbrIden cid, DbrDate settl_date)
{
    const DbrIden key = dbr_book_key(cid, settl_date);
	struct DbrRbNode* node = dbr_tree_find(&serv->books, key);
    return node ? serv_book_entry(node) : NULL;
}

static struct DbrBook*
lazy_book(DbrServ serv, struct DbrRec* crec, DbrDate settl_date)
{
    assert(crec);
    assert(crec->type == DBR_ENTITY_CONTR);

    const DbrIden key = dbr_book_key(crec->id, settl_date);
    struct DbrBook* book;
	struct DbrRbNode* node = dbr_tree_pfind(&serv->books, key);
    if (!node || node->key != key) {
        book = dbr_pool_alloc_book(serv->pool);
        if (dbr_unlikely(!book)) {
            dbr_err_set(DBR_ENOMEM, "out of memory");
            return NULL;
        }
        dbr_book_init(book, crec, settl_date, serv->pool);
        struct DbrRbNode* parent = node;
        dbr_tree_pinsert(&serv->books, key, &book->serv_node_, parent);
    } else
        book = serv_book_entry(node);
    return book;
}

static void
free_books(struct DbrTree* books, DbrPool pool)
{
    assert(books);
    struct DbrRbNode* node;
    while ((node = books->root)) {
        struct DbrBook* book = serv_book_entry(node);
        dbr_tree_remove(books, node);
        dbr_book_term(book);
        dbr_pool_free_book(pool, book);
    }
}

static void
free_match_list(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrMatch* match = dbr_trans_match_entry(node);
        node = node->next;
        // Not committed so match object still owns the trades.
        dbr_exec_decref(match->taker_exec, pool);
        dbr_exec_decref(match->maker_exec, pool);
        dbr_pool_free_match(pool, match);
    }
}

static DbrBool
emplace_rec_list(DbrServ serv, DbrModel model, int type)
{
    struct DbrSlNode* node;
    ssize_t size = dbr_model_read_entity(model, type, serv->pool, &node);
    if (size < 0)
        return DBR_FALSE;

    fig_cache_emplace_rec_list(&serv->cache, type, node, size);
    return DBR_TRUE;
}

static DbrBool
emplace_membs(DbrServ serv, DbrModel model)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(model, DBR_ENTITY_MEMB, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrMemb* umemb = enrich_memb(&serv->cache, dbr_shared_memb_entry(node));

        // Clone member for group.
        struct DbrMemb* gmemb = dbr_pool_alloc_memb(serv->pool);
        if (!gmemb)
            goto fail2;
        dbr_memb_init(gmemb);
        gmemb->user.rec = umemb->user.rec;
        gmemb->group.rec = umemb->group.rec;

        struct FigAccnt* user = fig_accnt_lazy(umemb->user.rec, &serv->ordidx, serv->pool);
        if (dbr_unlikely(!user)) {
            dbr_pool_free_memb(serv->pool, gmemb);
            goto fail2;
        }

        struct FigAccnt* group = fig_accnt_lazy(umemb->group.rec, &serv->ordidx, serv->pool);
        if (dbr_unlikely(!group)) {
            dbr_pool_free_memb(serv->pool, gmemb);
            goto fail2;
        }

        // Transfer ownership.
        fig_accnt_emplace_user(group, umemb);
        fig_accnt_emplace_group(user, gmemb);
    }
    return DBR_TRUE;
 fail2:
    // Free tail.
    do {
        struct DbrMemb* memb = dbr_shared_memb_entry(node);
        node = node->next;
        dbr_pool_free_memb(serv->pool, memb);
    } while (node);
 fail1:
    return DBR_FALSE;
}

static DbrBool
emplace_orders(DbrServ serv, DbrModel model)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(model, DBR_ENTITY_ORDER, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrOrder* order = enrich_order(&serv->cache, dbr_shared_order_entry(node));
        struct DbrBook* book;
        if (!dbr_order_done(order)) {

            book = lazy_book(serv, order->c.contr.rec, order->c.settl_date);
            if (dbr_unlikely(!book))
                goto fail2;

            if (dbr_unlikely(!dbr_book_insert(book, order)))
                goto fail2;
        } else
            book = NULL;

        struct FigAccnt* user = fig_accnt_lazy(order->c.user.rec, &serv->ordidx, serv->pool);
        if (dbr_unlikely(!user)) {
            if (book)
                dbr_book_remove(book, order);
            goto fail2;
        }

        // Transfer ownership.
        fig_accnt_emplace_order(user, order);
    }
    return DBR_TRUE;
 fail2:
    // Free tail.
    do {
        struct DbrOrder* order = dbr_shared_order_entry(node);
        node = node->next;
        dbr_pool_free_order(serv->pool, order);
    } while (node);
 fail1:
    return DBR_FALSE;
}

static DbrBool
emplace_trades(DbrServ serv, DbrModel model)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(model, DBR_ENTITY_EXEC, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrExec* exec = enrich_trade(&serv->cache, dbr_shared_exec_entry(node));
        struct FigAccnt* user = fig_accnt_lazy(exec->c.user.rec, &serv->ordidx, serv->pool);
        if (dbr_unlikely(!user))
            goto fail2;

        // Transfer ownership.
        fig_accnt_insert_trade(user, exec);
        dbr_exec_decref(exec, serv->pool);
    }
    return DBR_TRUE;
 fail2:
    // Free tail.
    do {
        struct DbrExec* exec = dbr_shared_exec_entry(node);
        node = node->next;
        dbr_exec_decref(exec, serv->pool);
    } while (node);
 fail1:
    return DBR_FALSE;
}

static DbrBool
emplace_posns(DbrServ serv, DbrModel model)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(model, DBR_ENTITY_POSN, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrPosn* posn = enrich_posn(&serv->cache, dbr_shared_posn_entry(node));
        struct FigAccnt* accnt = fig_accnt_lazy(posn->accnt.rec, &serv->ordidx, serv->pool);
        if (dbr_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        fig_accnt_emplace_posn(accnt, posn);
    }
    return DBR_TRUE;
 fail2:
    // Free tail.
    do {
        struct DbrPosn* posn = dbr_shared_posn_entry(node);
        node = node->next;
        dbr_pool_free_posn(serv->pool, posn);
    } while (node);
 fail1:
    return DBR_FALSE;
}

static struct DbrExec*
create_exec(DbrServ serv, struct DbrOrder* order, DbrMillis now)
{
    struct DbrExec* exec = dbr_pool_alloc_exec(serv->pool);
    if (!exec)
        return NULL;
    dbr_exec_init(exec);

    exec->id = dbr_bank_add_fetch(&serv->bank, DBR_REG_EXEC, 1L);
    exec->order = order->id;
    __builtin_memcpy(&exec->c, &order->c, sizeof(struct DbrCommon));
    exec->match = 0;
    exec->role = 0;
    exec->cpty.id_only = 0;
    exec->created = now;
    return exec;
}

static inline void
apply_posn(struct DbrPosn* posn, const struct DbrExec* exec)
{
    const double licks = exec->c.last_lots * exec->c.last_ticks;
    if (exec->c.action == DBR_ACTION_BUY) {
        posn->buy_licks += licks;
        posn->buy_lots += exec->c.last_lots;
    } else {
        assert(exec->c.action == DBR_ACTION_SELL);
        posn->sell_licks += licks;
        posn->sell_lots += exec->c.last_lots;
    }
}

// Assumes that maker lots have not been reduced since matching took place.

static void
commit_trans(DbrServ serv, struct FigAccnt* taker, struct DbrBook* book,
             struct DbrTrans* trans, DbrMillis now)
{
    while (!dbr_queue_empty(&trans->matches)) {

        struct DbrSlNode* node = dbr_queue_pop(&trans->matches);
        struct DbrMatch* match = dbr_trans_match_entry(node);
        struct DbrOrder* maker_order = match->maker_order;

        // Reduce maker.
        dbr_book_take(book, maker_order, match->lots, now);
        insert_posnup(&serv->posnups, match->maker_posn);

        // Must succeed because maker order exists.
        struct FigAccnt* maker = fig_accnt_lazy(maker_order->c.user.rec, &serv->ordidx,
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
        dbr_pool_free_match(serv->pool, match);
    }

    dbr_queue_join(&serv->execs, &trans->execs);
    if (trans->taker_posn)
        insert_posnup(&serv->posnups, trans->taker_posn);
}

DBR_API DbrServ
dbr_serv_create(const char* bank, DbrJourn journ, DbrPool pool)
{
    // 1.
    DbrServ serv = malloc(sizeof(struct FigServ));
    if (dbr_unlikely(!serv)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    // 2.
    if (!dbr_bank_init(&serv->bank, bank, DBR_REG_LEN))
        goto fail2;

    serv->journ = journ;
    serv->pool = pool;
    // 3.
    fig_cache_init(&serv->cache, term_state, pool);
    fig_ordidx_init(&serv->ordidx);
    // 4.
    fig_sessidx_init(&serv->sessidx, pool);
    // 5.
    dbr_tree_init(&serv->books);

    dbr_queue_init(&serv->execs);
    dbr_tree_init(&serv->posnups);
    dbr_tree_init(&serv->bookups);

    return serv;
 fail2:
    free(serv);
 fail1:
    return NULL;
}

DBR_API void
dbr_serv_destroy(DbrServ serv)
{
    if (serv) {
        // Ensure that executions are freed.
        dbr_serv_clear(serv);
        // 5.
        free_books(&serv->books, serv->pool);
        // 4.
        fig_sessidx_term(&serv->sessidx);
        // 3.
        fig_cache_term(&serv->cache);
        // 2.
        dbr_bank_term(&serv->bank);
        // 1.
        free(serv);
    }
}

// Cache

DBR_API DbrBool
dbr_serv_load(DbrServ serv, DbrModel model)
{
    return emplace_rec_list(serv, model, DBR_ENTITY_ACCNT)
        && emplace_rec_list(serv, model, DBR_ENTITY_CONTR)
        && emplace_membs(serv, model)
        && emplace_orders(serv, model)
        && emplace_trades(serv, model)
        && emplace_posns(serv, model);
}

DBR_API struct DbrSlNode*
dbr_serv_find_rec_id(DbrServ serv, int type, DbrIden id)
{
    return fig_cache_find_rec_id(&serv->cache, type, id);
}

DBR_API struct DbrSlNode*
dbr_serv_find_rec_mnem(DbrServ serv, int type, const char* mnem)
{
    return fig_cache_find_rec_mnem(&serv->cache, type, mnem);
}

DBR_API struct DbrSlNode*
dbr_serv_first_rec(DbrServ serv, int type, size_t* size)
{
    return fig_cache_first_rec(&serv->cache, type, size);
}

DBR_API DbrBool
dbr_serv_empty_rec(DbrServ serv, int type)
{
    return fig_cache_empty_rec(&serv->cache, type);
}

// Pool

DBR_API DbrAccnt
dbr_serv_accnt(DbrServ serv, struct DbrRec* arec)
{
    return fig_accnt_lazy(arec, &serv->ordidx, serv->pool);
}

DBR_API struct DbrBook*
dbr_serv_book(DbrServ serv, struct DbrRec* crec, DbrDate settl_date)
{
    return lazy_book(serv, crec, settl_date);
}

DBR_API struct DbrSess*
dbr_serv_sess(DbrServ serv, const DbrUuid uuid)
{
    return fig_sessidx_lazy(&serv->sessidx, uuid);
}

// Exec

DBR_API struct DbrOrder*
dbr_serv_place(DbrServ serv, DbrAccnt user, DbrAccnt group, struct DbrBook* book,
               const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min_lots)
{
    if (!fig_accnt_logged_on(user)) {
        dbr_err_setf(DBR_EEXIST, "not logged-on '%.16s'", user->rec->mnem);
        goto fail1;
    }

    if (lots == 0 || lots < min_lots) {
        dbr_err_setf(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    struct DbrOrder* new_order = dbr_pool_alloc_order(serv->pool);
    if (!new_order)
        goto fail1;
    dbr_order_init(new_order);

    new_order->level = NULL;
    new_order->id = dbr_bank_add_fetch(&serv->bank, DBR_REG_ORDER, 1L);
    new_order->c.user.rec = fig_accnt_rec(user);
    new_order->c.group.rec = fig_accnt_rec(group);
    new_order->c.contr.rec = book->crec;
    new_order->c.settl_date = book->settl_date;
    if (ref)
        strncpy(new_order->c.ref, ref, DBR_REF_MAX);
    else
        new_order->c.ref[0] = '\0';
    new_order->c.state = DBR_STATE_NEW;
    new_order->c.action = action;
    new_order->c.ticks = ticks;
    new_order->c.lots = lots;
    new_order->c.resd = lots;
    new_order->c.exec = 0;
    new_order->c.last_ticks = 0;
    new_order->c.last_lots = 0;
    new_order->c.min_lots = min_lots;
    new_order->created = now;
    new_order->modified = now;

    struct DbrExec* new_exec = create_exec(serv, new_order, now);
    if (!new_exec)
        goto fail2;

    struct DbrTrans trans;
    dbr_trans_init(&trans);

    dbr_queue_insert_front(&trans.execs, &new_exec->shared_node_);

    // Order fields are updated on match.
    if (!fig_match_orders(book, new_order, &serv->bank, serv->journ, &serv->ordidx,
                          serv->pool, &trans))
        goto fail3;

    // Place incomplete order in book.
    if (!dbr_order_done(new_order) && !dbr_book_insert(book, new_order))
        goto fail4;

    // TODO: IOC orders would need an additional revision for the unsolicited cancellation of any
    // unfilled quantity.

    if (!dbr_journ_insert_exec_list(serv->journ, trans.execs.first, DBR_TRUE))
        goto fail5;

    // Final commit phase cannot fail.
    fig_accnt_emplace_order(user, new_order);
    insert_bookup(&serv->bookups, book);
    // Commit trans to cycle and free matches.
    commit_trans(serv, user, book, &trans, now);
    return new_order;
 fail5:
    if (!dbr_order_done(new_order))
        dbr_book_remove(book, new_order);
 fail4:
    free_match_list(dbr_queue_first(&trans.matches), serv->pool);
 fail3:
    dbr_exec_decref(new_exec, serv->pool);
 fail2:
    dbr_pool_free_order(serv->pool, new_order);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_serv_revise_id(DbrServ serv, DbrAccnt user, DbrIden id, DbrLots lots)
{
    if (!fig_accnt_logged_on(user)) {
        dbr_err_setf(DBR_EEXIST, "not logged-on '%.16s'", user->rec->mnem);
        goto fail1;
    }

    struct DbrRbNode* node = fig_accnt_find_order_id(user, id);
    if (!node) {
        dbr_err_setf(DBR_EINVAL, "no such order '%ld'", id);
        goto fail1;
    }

    struct DbrOrder* order = dbr_accnt_order_entry(node);
    if (dbr_order_done(order)) {
        dbr_err_setf(DBR_EINVAL, "order complete '%ld'", id);
        goto fail1;
    }

    // Revised lots must not be:
    // 1. less than min lots;
    // 2. less than executed lots;
    // 3. greater than original lots.

    if (lots == 0 || lots < order->c.min_lots || lots < order->c.exec || lots > order->c.lots) {
        dbr_err_setf(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    struct DbrExec* exec = create_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Revise.
    exec->c.state = DBR_STATE_REVISE;
    const DbrLots delta = exec->c.lots - lots;
    assert(delta >= 0);
    exec->c.lots = lots;
    exec->c.resd -= delta;

    if (!dbr_journ_insert_exec(serv->journ, exec, DBR_TRUE))
        goto fail2;

    // Final commit phase cannot fail.
    struct DbrBook* book = get_book(serv, order->c.contr.rec->id, order->c.settl_date);
    assert(book);
    dbr_book_revise(book, order, lots, now);
    insert_bookup(&serv->bookups, book);
    dbr_queue_insert_back(&serv->execs, &exec->shared_node_);
    return order;
 fail2:
    dbr_exec_decref(exec, serv->pool);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_serv_revise_ref(DbrServ serv, DbrAccnt user, const char* ref, DbrLots lots)
{
    if (!fig_accnt_logged_on(user)) {
        dbr_err_setf(DBR_EEXIST, "not logged-on '%.16s'", user->rec->mnem);
        goto fail1;
    }

    struct DbrOrder* order = fig_accnt_find_order_ref(user, ref);
    if (!order) {
        dbr_err_setf(DBR_EINVAL, "no such order '%.64s'", ref);
        goto fail1;
    }

    if (dbr_order_done(order)) {
        dbr_err_setf(DBR_EINVAL, "order complete '%.64s'", ref);
        goto fail1;
    }

    // Revised lots must not be:
    // 1. less than min lots;
    // 2. less than executed lots;
    // 3. greater than original lots.

    if (lots == 0 || lots < order->c.min_lots || lots < order->c.exec || lots > order->c.lots) {
        dbr_err_setf(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    struct DbrExec* exec = create_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Revise.
    exec->c.state = DBR_STATE_REVISE;
    const DbrLots delta = exec->c.lots - lots;
    assert(delta >= 0);
    exec->c.lots = lots;
    exec->c.resd -= delta;

    if (!dbr_journ_insert_exec(serv->journ, exec, DBR_TRUE))
        goto fail2;

    // Final commit phase cannot fail.
    struct DbrBook* book = get_book(serv, order->c.contr.rec->id, order->c.settl_date);
    assert(book);
    dbr_book_revise(book, order, lots, now);
    insert_bookup(&serv->bookups, book);
    dbr_queue_insert_back(&serv->execs, &exec->shared_node_);
    return order;
 fail2:
    dbr_exec_decref(exec, serv->pool);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_serv_cancel_id(DbrServ serv, DbrAccnt user, DbrIden id)
{
    if (!fig_accnt_logged_on(user)) {
        dbr_err_setf(DBR_EEXIST, "not logged-on '%.16s'", user->rec->mnem);
        goto fail1;
    }

    struct DbrRbNode* node = fig_accnt_find_order_id(user, id);
    if (!node) {
        dbr_err_setf(DBR_EINVAL, "no such order '%ld'", id);
        goto fail1;
    }

    struct DbrOrder* order = dbr_accnt_order_entry(node);
    if (dbr_order_done(order)) {
        dbr_err_setf(DBR_EINVAL, "order complete '%ld'", id);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    struct DbrExec* exec = create_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Cancel.
    exec->c.state = DBR_STATE_CANCEL;
    exec->c.resd = 0;

    if (!dbr_journ_insert_exec(serv->journ, exec, DBR_TRUE))
        goto fail2;

    // Final commit phase cannot fail.
    struct DbrBook* book = get_book(serv, order->c.contr.rec->id, order->c.settl_date);
    assert(book);
    dbr_book_cancel(book, order, now);
    insert_bookup(&serv->bookups, book);
    dbr_queue_insert_back(&serv->execs, &exec->shared_node_);
    return order;
 fail2:
    dbr_exec_decref(exec, serv->pool);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_serv_cancel_ref(DbrServ serv, DbrAccnt user, const char* ref)
{
    if (!fig_accnt_logged_on(user)) {
        dbr_err_setf(DBR_EEXIST, "not logged-on '%.16s'", user->rec->mnem);
        goto fail1;
    }

    struct DbrOrder* order = fig_accnt_find_order_ref(user, ref);
    if (!order) {
        dbr_err_setf(DBR_EINVAL, "no such order '%.64s'", ref);
        goto fail1;
    }

    if (dbr_order_done(order)) {
        dbr_err_setf(DBR_EINVAL, "order complete '%.64s'", ref);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    struct DbrExec* exec = create_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Cancel.
    exec->c.state = DBR_STATE_CANCEL;
    exec->c.resd = 0;

    if (!dbr_journ_insert_exec(serv->journ, exec, DBR_TRUE))
        goto fail2;

    // Final commit phase cannot fail.
    struct DbrBook* book = get_book(serv, order->c.contr.rec->id, order->c.settl_date);
    assert(book);
    dbr_book_cancel(book, order, now);
    insert_bookup(&serv->bookups, book);
    dbr_queue_insert_back(&serv->execs, &exec->shared_node_);
    return order;
 fail2:
    dbr_exec_decref(exec, serv->pool);
 fail1:
    return NULL;
}

DBR_API DbrBool
dbr_serv_ack_trade(DbrServ serv, DbrAccnt user, DbrIden id)
{
    if (!fig_accnt_logged_on(user)) {
        dbr_err_setf(DBR_EEXIST, "not logged-on '%.16s'", user->rec->mnem);
        goto fail1;
    }

    struct DbrRbNode* node = fig_accnt_find_trade_id(user, id);
    if (!node) {
        dbr_err_setf(DBR_EINVAL, "no such trade '%ld'", id);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_update_exec(serv->journ, node->key, now))
        goto fail1;

    // No need to update timestamps on trade because it is immediately freed.

    struct DbrExec* exec = dbr_accnt_trade_entry(node);
    fig_accnt_remove_trade(user, exec);
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

DBR_API void
dbr_serv_mdclear(DbrServ serv)
{
    dbr_tree_init(&serv->bookups);
}

DBR_API void
dbr_serv_trclear(DbrServ serv)
{
    struct DbrSlNode* node = serv->execs.first;
    while (node) {
        struct DbrExec* exec = dbr_serv_exec_entry(node);
        node = node->next;
        // Free completed orders.
        if (dbr_exec_done(exec)) {
            DbrAccnt user = exec->c.user.rec->accnt.state;
            assert(user);
            struct DbrOrder* order = fig_accnt_release_order_id(user, exec->order);
            if (order)
                dbr_pool_free_order(serv->pool, order);
        }
        dbr_exec_decref(exec, serv->pool);
    }
    dbr_queue_init(&serv->execs);
    dbr_tree_init(&serv->posnups);
}

DBR_API struct DbrSlNode*
dbr_serv_first_exec(DbrServ serv)
{
    return dbr_queue_first(&serv->execs);
}

DBR_API DbrBool
dbr_serv_empty_exec(DbrServ serv)
{
    return dbr_queue_empty(&serv->execs);
}

DBR_API struct DbrRbNode*
dbr_serv_first_posnup(DbrServ serv)
{
    return dbr_tree_first(&serv->posnups);
}

DBR_API struct DbrRbNode*
dbr_serv_last_posnup(DbrServ serv)
{
    return dbr_tree_last(&serv->posnups);
}

DBR_API DbrBool
dbr_serv_empty_posnup(DbrServ serv)
{
    return dbr_tree_empty(&serv->posnups);
}

// Book

DBR_API struct DbrRbNode*
dbr_serv_find_book(DbrServ serv, DbrIden cid, DbrDate settl_date)
{
    const DbrIden key = dbr_book_key(cid, settl_date);
	return dbr_tree_find(&serv->books, key);
}

DBR_API struct DbrRbNode*
dbr_serv_first_book(DbrServ serv)
{
    return dbr_tree_first(&serv->books);
}

DBR_API struct DbrRbNode*
dbr_serv_last_book(DbrServ serv)
{
    return dbr_tree_last(&serv->books);
}

DBR_API DbrBool
dbr_serv_empty_book(DbrServ serv)
{
    return dbr_tree_empty(&serv->books);
}

DBR_API struct DbrRbNode*
dbr_serv_first_bookup(DbrServ serv)
{
    return dbr_tree_first(&serv->bookups);
}

DBR_API struct DbrRbNode*
dbr_serv_last_bookup(DbrServ serv)
{
    return dbr_tree_last(&serv->bookups);
}

DBR_API DbrBool
dbr_serv_empty_bookup(DbrServ serv)
{
    return dbr_tree_empty(&serv->bookups);
}
