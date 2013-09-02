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
    ssize_t
    (*read_entity)(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first);
};

/**
 * @brief Read entity list for type.
 *
 * The caller is responsible for freeing the resulting list.
 */

static inline ssize_t
dbr_model_read_entity(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first)
{
    return model->vtbl->read_entity(model, type, pool, first);
}

static inline struct DbrRec*
dbr_model_rec_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrRec, model_node_, node);
}

static inline struct DbrOrder*
dbr_model_order_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrOrder, model_node_, node);
}

static inline struct DbrMemb*
dbr_model_memb_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrMemb, model_node_, node);
}

static inline struct DbrTrade*
dbr_model_trade_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrTrade, model_node_, node);
}

static inline struct DbrPosn*
dbr_model_posn_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrPosn, model_node_, node);
}

/** @} */

#endif // DBR_MODEL_H
