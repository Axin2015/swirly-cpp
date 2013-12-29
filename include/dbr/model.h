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
 * Read entity list for type.
 * The caller is responsible for freeing the resulting list.
 */

static inline ssize_t
dbr_model_read_entity(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first)
{
    return model->vtbl->read_entity(model, type, pool, first);
}

/** @} */

#endif // DBR_MODEL_H
