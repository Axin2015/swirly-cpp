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

#include <dbr/types.h>

/**
 * @addtogroup Model
 * @{
 */

struct DbrModelVtbl {
    ssize_t
    (*read_entity)(DbrModel model, int type, struct DbrSlNode** first);

    struct DbrSlNode*
    (*end_entity)(DbrModel model);
};

static inline ssize_t
dbr_model_read_entity(DbrModel model, int type, struct DbrSlNode** first)
{
    return model->vtbl->read_entity(model, type, first);
}

static inline struct DbrSlNode*
dbr_model_end_entity(DbrModel model)
{
    return model->vtbl->end_entity(model);
}

/** @} */

#endif // DBR_MODEL_H
