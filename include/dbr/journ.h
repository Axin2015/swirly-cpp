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
#ifndef DBR_JOURN_H
#define DBR_JOURN_H

#include <dbr/types.h>

/**
 * @addtogroup Journ
 * @{
 */

typedef struct DbrIJourn {
    const struct DbrJournVtbl* vtbl;
}* DbrJourn;

struct DbrJournVtbl {
    void
    (*destroy)(DbrJourn journ);

    DbrBool
    (*insert_exec_list)(DbrJourn journ, struct DbrSlNode* first, DbrBool enriched);

    DbrBool
    (*insert_exec)(DbrJourn journ, struct DbrExec* exec, DbrBool enriched);

    DbrBool
    (*update_exec)(DbrJourn journ, DbrIden id, DbrMillis modified);
};

static inline void
dbr_journ_destroy(DbrJourn journ)
{
    journ->vtbl->destroy(journ);
}

static inline DbrBool
dbr_journ_insert_exec_list(DbrJourn journ, struct DbrSlNode* first, DbrBool enriched)
{
    return journ->vtbl->insert_exec_list(journ, first, enriched);
}

static inline DbrBool
dbr_journ_insert_exec(DbrJourn journ, struct DbrExec* exec, DbrBool enriched)
{
    return journ->vtbl->insert_exec(journ, exec, enriched);
}

static inline DbrBool
dbr_journ_update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    return journ->vtbl->update_exec(journ, id, modified);
}

/** @} */

#endif // DBR_JOURN_H
