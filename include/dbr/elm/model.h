/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_MODEL_H
#define DBR_ASH_MODEL_H

#include <dbr/elm/types.h>

/**
 * @addtogroup Model
 * @{
 */

typedef struct DbrIModel {
    const struct DbrModelVtbl* vtbl;
}* DbrModel;

struct DbrModelVtbl {
    void
    (*destroy)(DbrModel model);

    ssize_t
    (*read_entity)(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first);
};

static inline void
dbr_model_destroy(DbrModel model)
{
    model->vtbl->destroy(model);
}

/**
 * Read entity list for type.
 * The caller is responsible for freeing the resulting list.
 */

static inline ssize_t
dbr_model_read_entity(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first)
{
    return model->vtbl->read_entity(model, type, pool, first);
}

/** @} */

#endif // DBR_ASH_MODEL_H
