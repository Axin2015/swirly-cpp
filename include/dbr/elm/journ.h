/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_JOURN_H
#define DBR_ASH_JOURN_H

#include <dbr/elm/types.h>

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

#endif // DBR_ASH_JOURN_H
