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
#ifndef DBR_MODEL_H
#define DBR_MODEL_H

#include <dbr/pool.h>
#include <dbr/types.h>

/**
 * @addtogroup Model
 * @{
 */

struct DbrModelVtbl {
    DbrIden
    (*alloc_id)(DbrModel model);

    DbrBool
    (*begin_trans)(DbrModel model);

    DbrBool
    (*commit_trans)(DbrModel model);

    DbrBool
    (*rollback_trans)(DbrModel model);

    DbrBool
    (*insert_order)(DbrModel model, struct DbrOrder* order);

    DbrBool
    (*update_order)(DbrModel model, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
                    DbrLots lots, DbrMillis now);

    DbrBool
    (*archive_order)(DbrModel model, DbrIden id, DbrMillis now);

    DbrBool
    (*insert_trade)(DbrModel model, struct DbrTrade* trade);

    DbrBool
    (*archive_trade)(DbrModel model, DbrIden id, DbrMillis now);

    ssize_t
    (*select_entity)(DbrModel model, int type, struct DbrSlNode** first);

    struct DbrSlNode*
    (*end_entity)(DbrModel model);
};

static inline DbrIden
dbr_model_alloc_id(DbrModel model)
{
    return model->vtbl->alloc_id(model);
}

static inline DbrBool
dbr_model_begin_trans(DbrModel model)
{
    return model->vtbl->begin_trans(model);
}

static inline DbrBool
dbr_model_commit_trans(DbrModel model)
{
    return model->vtbl->commit_trans(model);
}

static inline DbrBool
dbr_model_rollback_trans(DbrModel model)
{
    return model->vtbl->rollback_trans(model);
}

static inline DbrBool
dbr_model_insert_order(DbrModel model, struct DbrOrder* order)
{
    return model->vtbl->insert_order(model, order);
}

static inline DbrBool
dbr_model_update_order(DbrModel model, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
                       DbrLots lots, DbrMillis now)
{
    return model->vtbl->update_order(model, id, rev, status, resd, exec, lots, now);
}

static inline DbrBool
dbr_model_archive_order(DbrModel model, DbrIden id, DbrMillis now)
{
    return model->vtbl->archive_order(model, id, now);
}

static inline DbrBool
dbr_model_insert_trade(DbrModel model, struct DbrTrade* trade)
{
    return model->vtbl->insert_trade(model, trade);
}

static inline DbrBool
dbr_model_archive_trade(DbrModel model, DbrIden id, DbrMillis now)
{
    return model->vtbl->archive_trade(model, id, now);
}

static inline ssize_t
dbr_model_select_entity(DbrModel model, int type, struct DbrSlNode** first)
{
    return model->vtbl->select_entity(model, type, first);
}

static inline struct DbrSlNode*
dbr_model_end_entity(DbrModel model)
{
    return model->vtbl->end_entity(model);
}

/** @} */

#if !defined(DBR_HIDE_API)

/**
 * @addtogroup Sqlite
 * @{
 */

/**
 * @brief Create SQLite3 model.
 *
 * @param pool Pool.
 *
 * @param seed Seed identifier.
 *
 * @param path Path to the database.
 *
 * @return Handle to newly created model or null on failure.
 */

DBR_API DbrModel
dbr_sqlite_create(DbrPool pool, DbrIden seed, const char* path);

DBR_API void
dbr_sqlite_destroy(DbrModel model);

/** @} */

#endif // !defined(DBR_HIDE_API)

#endif // DBR_MODEL_H
