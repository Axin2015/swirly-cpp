/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_JOURN_H
#define SC_ASH_JOURN_H

#include <sc/elm/types.h>

/**
 * @addtogroup Journ
 * @{
 */

typedef struct ScIJourn {
    const struct ScJournVtbl* vtbl;
}* ScJourn;

struct ScJournVtbl {
    void
    (*destroy)(ScJourn journ);

    ScBool
    (*insert_exec_list)(ScJourn journ, struct ScSlNode* first, ScBool enriched);

    ScBool
    (*insert_exec)(ScJourn journ, struct ScExec* exec, ScBool enriched);

    ScBool
    (*update_exec)(ScJourn journ, ScIden id, ScMillis modified);
};

static inline void
sc_journ_destroy(ScJourn journ)
{
    journ->vtbl->destroy(journ);
}

static inline ScBool
sc_journ_insert_exec_list(ScJourn journ, struct ScSlNode* first, ScBool enriched)
{
    return journ->vtbl->insert_exec_list(journ, first, enriched);
}

static inline ScBool
sc_journ_insert_exec(ScJourn journ, struct ScExec* exec, ScBool enriched)
{
    return journ->vtbl->insert_exec(journ, exec, enriched);
}

static inline ScBool
sc_journ_update_exec(ScJourn journ, ScIden id, ScMillis modified)
{
    return journ->vtbl->update_exec(journ, id, modified);
}

/** @} */

#endif // SC_ASH_JOURN_H
