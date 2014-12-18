/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_MODEL_H
#define SC_ASH_MODEL_H

#include <sc/elm/types.h>

/**
 * @addtogroup Model
 * @{
 */

typedef struct ScIModel {
    const struct ScModelVtbl* vtbl;
}* ScModel;

struct ScModelVtbl {
    void
    (*destroy)(ScModel model);

    ssize_t
    (*read_entity)(ScModel model, int type, ScPool pool, struct ScSlNode** first);
};

static inline void
sc_model_destroy(ScModel model)
{
    model->vtbl->destroy(model);
}

/**
 * Read entity list for type.
 * The caller is responsible for freeing the resulting list.
 */

static inline ssize_t
sc_model_read_entity(ScModel model, int type, ScPool pool, struct ScSlNode** first)
{
    return model->vtbl->read_entity(model, type, pool, first);
}

/** @} */

#endif // SC_ASH_MODEL_H
