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
#include <dbr/fig/sess.h>

#include "accnt.h"

#include <dbr/ash/err.h>

#include <string.h> // strncpy()

#include <uuid/uuid.h>

static int
incref(struct DbrSess* sess, DbrKey key)
{
    struct DbrSub* sub;
    struct DbrRbNode* node = dbr_tree_pfind(&sess->subs, key);
    if (!node || node->key != key) {
        if (!(sub = dbr_pool_alloc_sub(sess->pool)))
            return -1;
        dbr_sub_init(sub);

        struct DbrRbNode* parent = node;
        dbr_tree_pinsert(&sess->subs, key, &sub->sess_node_, parent);
    } else {
        sub = dbr_sess_sub_entry(node);
        ++sub->refs_;
    }
    return sub->refs_;
}

static int
decref(struct DbrSess* sess, DbrKey key)
{
    int refs;
    struct DbrRbNode* node = dbr_tree_find(&sess->subs, key);
    if (node) {
        struct DbrSub* sub = dbr_sess_sub_entry(node);
        refs = --sub->refs_;
        if (refs == 0) {
            dbr_tree_remove(&sess->subs, node);
            dbr_pool_free_sub(sess->pool, sub);
        }
    } else
        refs = 0;
    return refs;
}

DBR_API DbrAccnt
dbr_sess_accnt_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct FigAccnt, sess_node_, node);
}

DBR_API void
dbr_sess_init(struct DbrSess* sess, const DbrUuid uuid, DbrPool pool)
{
    if (uuid)
        uuid_copy(sess->uuid, uuid);
    else
        uuid_generate(sess->uuid);
    sess->pool = pool;
    sess->sid = 1;
    sess->hbint = 0;
    dbr_tree_init(&sess->subs);
    dbr_tree_init(&sess->accnts);
    sess->marker_ = 0;
    dbr_slnode_init(&sess->uuid_node_);
}

DBR_API void
dbr_sess_term(struct DbrSess* sess)
{
    struct DbrRbNode* node;
    while ((node = sess->subs.root)) {
        struct DbrSub* sub = dbr_sess_sub_entry(node);
        dbr_tree_remove(&sess->subs, node);
        dbr_pool_free_sub(sess->pool, sub);
    }
}

DBR_API void
dbr_sess_reset(struct DbrSess* sess)
{
    dbr_sess_term(sess);
    ++sess->sid;
    sess->hbint = 0;
    dbr_tree_init(&sess->subs);
    dbr_tree_init(&sess->accnts);
    sess->marker_ = 0;
    dbr_slnode_init(&sess->uuid_node_);
}

DBR_API DbrBool
dbr_sess_logon(struct DbrSess* sess, DbrAccnt accnt)
{
    if (accnt->sess) {
        dbr_err_setf(DBR_EEXIST, "already logged-on '%.16s'", accnt->rec->mnem);
        goto fail1;
    }

    if (incref(sess, accnt->rec->id) < 0)
        goto fail1;

    struct DbrRbNode* node = dbr_accnt_first_giveup(accnt);
    for (; node != DBR_ACCNT_END_GIVEUP; node = dbr_rbnode_next(node)) {
        struct DbrPerm* perm = dbr_accnt_giveup_entry(node);
        if (incref(sess, perm->giveup.rec->id) < 0)
            goto fail2;
    }

    accnt->sess = sess;
    dbr_tree_insert(&sess->accnts, accnt->rec->id, &accnt->sess_node_);
    return DBR_TRUE;
 fail2:
    // Rollback subs.
    for (node = dbr_rbnode_prev(node);
         node != DBR_ACCNT_END_GIVEUP; node = dbr_rbnode_prev(node)) {
        struct DbrPerm* perm = dbr_accnt_giveup_entry(node);
        decref(sess, perm->giveup.rec->id);
    }
    decref(sess, accnt->rec->id);
 fail1:
    return DBR_FALSE;
}

DBR_API void
dbr_sess_logoff(struct DbrSess* sess, DbrAccnt accnt)
{
    dbr_tree_remove(&sess->accnts, &accnt->sess_node_);
    accnt->sess = NULL;

    for (struct DbrRbNode* node = dbr_accnt_first_giveup(accnt);
         node != DBR_ACCNT_END_GIVEUP; node = dbr_rbnode_next(node)) {
        struct DbrPerm* perm = dbr_accnt_giveup_entry(node);
        decref(sess, perm->giveup.rec->id);
    }
    decref(sess, accnt->rec->id);
}

DBR_API void
dbr_sess_logoff_and_reset(struct DbrSess* sess, DbrAccnt accnt)
{
    dbr_tree_remove(&sess->accnts, &accnt->sess_node_);
    accnt->sess = NULL;

    for (struct DbrRbNode* node = dbr_accnt_first_giveup(accnt);
         node != DBR_ACCNT_END_GIVEUP; node = dbr_rbnode_next(node)) {
        struct DbrPerm* perm = dbr_accnt_giveup_entry(node);
        if (decref(sess, perm->giveup.rec->id) == 0) {
            DbrAccnt giveup = perm->giveup.rec->accnt.state;
            if (giveup)
                fig_accnt_reset_giveup(giveup);
        }
    }
    if (decref(sess, accnt->rec->id) == 0)
        fig_accnt_reset_giveup(accnt);
    fig_accnt_reset_trader(accnt);
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
