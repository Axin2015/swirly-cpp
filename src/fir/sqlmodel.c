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
#include <dbr/sqlmodel.h>

#include "sqlite.h"

#include <dbr/err.h>

#include <stdlib.h>

struct FirSqlModel {
    struct FirSqlite impl;
    struct DbrIModel model;
};

static inline struct FirSqlModel*
model_implof(DbrModel model)
{
    return dbr_implof(struct FirSqlModel, model, model);
}

static void
destroy(DbrModel model)
{
    struct FirSqlModel* model = model_implof(model);
    fir_sqlite_term(&model->impl);
    free(model);
}

static ssize_t
read_entity(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first)
{
    struct FirSqlModel* model = model_implof(model);
    struct FirSqlite* impl = &model->impl;
    return fir_sqlite_select_entity(impl, type, pool, first);
}

static const struct DbrModelVtbl MODEL_VTBL = {
    .destroy = destroy,
    .read_entity = read_entity
};

DBR_API DbrModel
dbr_sqlmodel_create(const char* path)
{
    struct FirSqlModel* model = malloc(sizeof(struct FirSqlModel));
    if (dbr_unlikely(!model)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    if (!fir_sqlite_init(&model->model, path))
        goto fail2;

    model->model.vtbl = &MODEL_VTBL;
    return &model->model;
 fail2:
    free(model);
 fail1:
    return NULL;
}
