/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/fir/sqlmodel.h>

#include "sqlite.h"

#include <sc/ash/err.h>

#include <stdlib.h>

struct SqlModel {
    struct FirSqlite sqlite;
    struct ScIModel i_model;
};

static inline struct SqlModel*
model_implof(ScModel model)
{
    return sc_implof(struct SqlModel, i_model, model);
}

static void
destroy(ScModel model)
{
    struct SqlModel* impl = model_implof(model);
    fir_sqlite_term(&impl->sqlite);
    free(impl);
}

static ssize_t
read_entity(ScModel model, int type, ScPool pool, struct ScSlNode** first)
{
    struct SqlModel* impl = model_implof(model);
    struct FirSqlite* sqlite = &impl->sqlite;
    return fir_sqlite_select_entity(sqlite, type, pool, first);
}

static const struct ScModelVtbl MODEL_VTBL = {
    .destroy = destroy,
    .read_entity = read_entity
};

SC_API ScModel
sc_sqlmodel_create(const char* path)
{
    struct SqlModel* impl = malloc(sizeof(struct SqlModel));
    if (sc_unlikely(!impl)) {
        sc_err_set(SC_ENOMEM, "out of memory");
        goto fail1;
    }

    if (!fir_sqlite_init(&impl->sqlite, path))
        goto fail2;

    impl->i_model.vtbl = &MODEL_VTBL;
    return &impl->i_model;
 fail2:
    free(impl);
 fail1:
    return NULL;
}
