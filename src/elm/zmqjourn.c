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
#include <dbr/zmqjourn.h>

#include <dbr/err.h>

#include <stdlib.h>

struct FirZmqJourn {
    struct DbrIJourn journ;
};

static inline struct FirZmqJourn*
journ_implof(DbrJourn journ)
{
    return dbr_implof(struct FirZmqJourn, journ, journ);
}

static void
destroy(DbrJourn journ)
{
    struct FirZmqJourn* impl = journ_implof(journ);
    free(impl);
}

static DbrBool
insert_exec_list(DbrJourn journ, struct DbrSlNode* first, DbrBool enriched)
{
    return DBR_TRUE;
}

static DbrBool
insert_exec(DbrJourn journ, struct DbrExec* exec, DbrBool enriched)
{
    return DBR_TRUE;
}

static DbrBool
update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    return DBR_TRUE;
}

static const struct DbrJournVtbl JOURN_VTBL = {
    .destroy = destroy,
    .insert_exec_list = insert_exec_list,
    .insert_exec = insert_exec,
    .update_exec = update_exec
};

DBR_API DbrJourn
dbr_zmqjourn_create(DbrJourn (*factory)(void*), void* arg)
{
    struct FirZmqJourn* impl = malloc(sizeof(struct FirZmqJourn));
    if (dbr_unlikely(!impl)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    // Seed identity.
    impl->journ.vtbl = &JOURN_VTBL;
    return &impl->journ;
 fail1:
    return NULL;
}
