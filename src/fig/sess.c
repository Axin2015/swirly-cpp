/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/fig/sess.h>

#include "accnt.h"

#include <sc/ash/err.h>

#include <string.h> // strncpy()

#include <uuid/uuid.h>

static int
incref(struct ScSess* sess, ScKey key)
{
    struct ScSub* sub;
    struct ScRbNode* node = sc_tree_pfind(&sess->subs, key);
    if (!node || node->key != key) {
        if (!(sub = sc_pool_alloc_sub(sess->pool)))
            return -1;
        sc_sub_init(sub);

        struct ScRbNode* parent = node;
        sc_tree_pinsert(&sess->subs, key, &sub->sess_node_, parent);
    } else {
        sub = sc_sess_sub_entry(node);
        ++sub->refs_;
    }
    return sub->refs_;
}

static int
decref(struct ScSess* sess, ScKey key)
{
    int refs;
    struct ScRbNode* node = sc_tree_find(&sess->subs, key);
    if (node) {
        struct ScSub* sub = sc_sess_sub_entry(node);
        refs = --sub->refs_;
        if (refs == 0) {
            sc_tree_remove(&sess->subs, node);
            sc_pool_free_sub(sess->pool, sub);
        }
    } else
        refs = 0;
    return refs;
}

SC_API ScAccnt
sc_sess_accnt_entry(struct ScRbNode* node)
{
    return sc_implof(struct FigAccnt, sess_node_, node);
}

SC_API void
sc_sess_init(struct ScSess* sess, const ScUuid uuid, ScPool pool)
{
    if (uuid)
        uuid_copy(sess->uuid, uuid);
    else
        uuid_generate(sess->uuid);
    sess->pool = pool;
    sess->sid = 1;
    sess->hbint = 0;
    sc_tree_init(&sess->subs);
    sc_tree_init(&sess->accnts);
    sess->marker_ = 0;
    sc_slnode_init(&sess->uuid_node_);
}

SC_API void
sc_sess_term(struct ScSess* sess)
{
    struct ScRbNode* node;
    while ((node = sess->subs.root)) {
        struct ScSub* sub = sc_sess_sub_entry(node);
        sc_tree_remove(&sess->subs, node);
        sc_pool_free_sub(sess->pool, sub);
    }
}

SC_API void
sc_sess_reset(struct ScSess* sess)
{
    sc_sess_term(sess);
    ++sess->sid;
    sess->hbint = 0;
    sc_tree_init(&sess->subs);
    sc_tree_init(&sess->accnts);
    sess->marker_ = 0;
    sc_slnode_init(&sess->uuid_node_);
}

SC_API ScBool
sc_sess_logon(struct ScSess* sess, ScAccnt accnt)
{
    if (accnt->sess) {
        sc_err_setf(SC_EEXIST, "already logged-on '%.16s'", accnt->rec->mnem);
        goto fail1;
    }

    if (incref(sess, accnt->rec->id) < 0)
        goto fail1;

    struct ScRbNode* node = sc_accnt_first_giveup(accnt);
    for (; node != SC_ACCNT_END_GIVEUP; node = sc_rbnode_next(node)) {
        struct ScPerm* perm = sc_accnt_giveup_entry(node);
        if (incref(sess, perm->giveup.rec->id) < 0)
            goto fail2;
    }

    accnt->sess = sess;
    sc_tree_insert(&sess->accnts, accnt->rec->id, &accnt->sess_node_);
    return SC_TRUE;
 fail2:
    // Rollback subs.
    for (node = sc_rbnode_prev(node);
         node != SC_ACCNT_END_GIVEUP; node = sc_rbnode_prev(node)) {
        struct ScPerm* perm = sc_accnt_giveup_entry(node);
        decref(sess, perm->giveup.rec->id);
    }
    decref(sess, accnt->rec->id);
 fail1:
    return SC_FALSE;
}

SC_API void
sc_sess_logoff(struct ScSess* sess, ScAccnt accnt)
{
    sc_tree_remove(&sess->accnts, &accnt->sess_node_);
    accnt->sess = NULL;

    for (struct ScRbNode* node = sc_accnt_first_giveup(accnt);
         node != SC_ACCNT_END_GIVEUP; node = sc_rbnode_next(node)) {
        struct ScPerm* perm = sc_accnt_giveup_entry(node);
        decref(sess, perm->giveup.rec->id);
    }
    decref(sess, accnt->rec->id);
}

SC_API void
sc_sess_logoff_and_reset(struct ScSess* sess, ScAccnt accnt)
{
    sc_tree_remove(&sess->accnts, &accnt->sess_node_);
    accnt->sess = NULL;

    for (struct ScRbNode* node = sc_accnt_first_giveup(accnt);
         node != SC_ACCNT_END_GIVEUP; node = sc_rbnode_next(node)) {
        struct ScPerm* perm = sc_accnt_giveup_entry(node);
        if (decref(sess, perm->giveup.rec->id) == 0) {
            ScAccnt giveup = perm->giveup.rec->accnt.state;
            if (giveup)
                fig_accnt_reset_giveup(giveup);
        }
    }
    if (decref(sess, accnt->rec->id) == 0)
        fig_accnt_reset_giveup(accnt);
    fig_accnt_reset_trader(accnt);
}

SC_API int
sc_sess_subs(struct ScSess* sess, ScAccnt accnt)
{
    int refs;
    struct ScRbNode* node = sc_tree_find(&sess->subs, accnt->rec->id);
    if (node) {
        struct ScSub* sub = sc_sess_sub_entry(node);
        refs = sub->refs_;
    } else
        refs = 0;
    return refs;
}
