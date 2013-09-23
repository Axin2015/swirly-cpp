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
#include <dbr/zmqstore.h>

#include <dbr/journ.h>
#include <dbr/model.h>

#include <stdbool.h>
#include <stdlib.h>

struct DbrZmqStore_ {
    DbrIden id;
    struct DbrIJourn journ_;
    struct DbrIModel model_;
};

static inline struct DbrZmqStore_*
journ_implof(DbrJourn journ)
{
    return dbr_implof(struct DbrZmqStore_, journ_, journ);
}

static inline struct DbrZmqStore_*
model_implof(DbrModel model)
{
    return dbr_implof(struct DbrZmqStore_, model_, model);
}

static DbrIden
alloc_id(DbrJourn journ)
{
    struct DbrZmqStore_* zmqstore = journ_implof(journ);
    return zmqstore->id++;
}

static DbrBool
begin_trans(DbrJourn journ)
{
    return true;
}

static DbrBool
commit_trans(DbrJourn journ)
{
    return true;
}

static DbrBool
rollback_trans(DbrJourn journ)
{
    return true;
}

static DbrBool
insert_order(DbrJourn journ, struct DbrOrder* order)
{
    return true;
}

static DbrBool
update_order(DbrJourn journ, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
             DbrLots lots, DbrMillis now)
{
    return true;
}

static DbrBool
archive_order(DbrJourn journ, DbrIden id, DbrMillis now)
{
    return true;
}

static DbrBool
insert_trade(DbrJourn journ, struct DbrTrade* trade)
{
    return true;
}

static DbrBool
archive_trade(DbrJourn journ, DbrIden id, DbrMillis now)
{
    return true;
}

static const struct DbrJournVtbl JOURN_VTBL = {
    .alloc_id = alloc_id,
    .begin_trans = begin_trans,
    .commit_trans = commit_trans,
    .rollback_trans = rollback_trans,
    .insert_order = insert_order,
    .update_order = update_order,
    .archive_order = archive_order,
    .insert_trade = insert_trade,
    .archive_trade = archive_trade
};

static ssize_t
read_entity(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first)
{
    return 0;
}

static const struct DbrModelVtbl MODEL_VTBL = {
    .read_entity = read_entity
};

DBR_API DbrZmqStore
dbr_zmqstore_create(DbrIden seed, const char* path)
{
    struct DbrZmqStore_* zmqstore = malloc(sizeof(struct DbrZmqStore_));
    if (dbr_unlikely(!zmqstore))
        goto fail1;

    // Seed identity.
    zmqstore->id = seed;

    zmqstore->journ_.vtbl = &JOURN_VTBL;
    zmqstore->model_.vtbl = &MODEL_VTBL;

    return zmqstore;
 fail1:
    return NULL;
}

DBR_API void
dbr_zmqstore_destroy(DbrZmqStore zmqstore)
{
    if (zmqstore) {
        free(zmqstore);
    }
}

DBR_API DbrJourn
dbr_zmqstore_journ(DbrZmqStore zmqstore)
{
    return &zmqstore->journ_;
}

DBR_API DbrModel
dbr_zmqstore_model(DbrZmqStore zmqstore)
{
    return &zmqstore->model_;
}
