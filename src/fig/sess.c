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
#include <dbr/sess.h>

#include "accnt.h"
#include "trader.h"

#include <dbr/err.h>

static DbrBool
incref(struct DbrSess* sess, DbrKey key, DbrPool pool)
{
    struct DbrSub* sub;
    struct DbrRbNode* node = dbr_tree_pfind(&sess->subs, key);
    if (!node || node->key != key) {
        if (!(sub = dbr_pool_alloc_sub(pool)))
            return DBR_FALSE;
        dbr_sub_init(sub);

        struct DbrRbNode* parent = node;
        dbr_tree_pinsert(&sess->subs, key, &sub->sess_node_, parent);
    } else {
        sub = dbr_sess_sub_entry(node);
        ++sub->refs_;
    }
    return DBR_TRUE;
}

static void
decref(struct DbrSess* sess, DbrKey key, DbrPool pool)
{
    struct DbrRbNode* node = dbr_tree_find(&sess->subs, key);
    if (node) {
        struct DbrSub* sub = dbr_sess_sub_entry(node);
        if (--sub->refs_ == 0) {
            dbr_tree_remove(&sess->subs, node);
            dbr_pool_free_sub(pool, sub);
        }
    }
}

DBR_API DbrTrader
dbr_sess_trader_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct FigTrader, sess_node_, node);
}

DBR_API DbrBool
dbr_sess_logon(struct DbrSess* sess, DbrTrader trader)
{
    if (trader->sess) {
        dbr_err_setf(DBR_EEXIST, "already logged-on '%.16s'", trader->rec->mnem);
        goto fail1;
    }

    struct DbrRbNode* node = dbr_trader_first_memb(trader);
    for (; node != DBR_TRADER_END_MEMB; node = dbr_rbnode_next(node)) {
        struct DbrMemb* memb = dbr_trader_memb_entry(node);
        if (!incref(sess, memb->accnt.rec->id, trader->pool))
            goto fail2;
    }

    trader->sess = sess;
    dbr_tree_insert(&sess->traders, trader->rec->id, &trader->sess_node_);
    return DBR_TRUE;
 fail2:
    // Rollback subs.
    for (node = dbr_rbnode_prev(node);
         node != DBR_TRADER_END_MEMB; node = dbr_rbnode_prev(node)) {
        struct DbrMemb* memb = dbr_trader_memb_entry(node);
        decref(sess, memb->accnt.rec->id, trader->pool);
    }
 fail1:
    return DBR_FALSE;
}

DBR_API void
dbr_sess_logoff(struct DbrSess* sess, DbrTrader trader)
{
    dbr_tree_remove(&sess->traders, &trader->sess_node_);
    trader->sess = NULL;

    for (struct DbrRbNode* node = dbr_trader_first_memb(trader);
         node != DBR_TRADER_END_MEMB; node = dbr_rbnode_next(node)) {
        struct DbrMemb* memb = dbr_trader_memb_entry(node);
        decref(sess, memb->accnt.rec->id, trader->pool);
    }
}

DBR_API int
dbr_sess_subs(struct DbrSess* sess, DbrAccnt accnt)
{
    int refs;
    struct DbrRbNode* node = dbr_tree_find(&sess->subs, accnt->rec->id);
    if (node) {
        struct DbrSub* sub = dbr_sess_sub_entry(node);
        refs = sub->refs_;
    } else
        refs = 0;
    return refs;
}
