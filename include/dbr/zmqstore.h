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
#ifndef DBR_ZMQSTORE_H
#define DBR_ZMQSTORE_H

#include <dbr/journ.h>
#include <dbr/model.h>
#include <dbr/types.h>

/**
 * @addtogroup Zmq
 * @{
 */

typedef struct ElmZmqStore* DbrZmqStore;

/**
 * Create ZMQite3 model.
 *
 * @param ctx ZeroMQ context.
 *
 * @param addr Address of peer's endpoint.
 *
 * @param seed Seed identifier. Must be positive.
 *
 * @param pool Pool.
 *
 * @return Handle to newly created model or null on failure.
 */

DBR_API DbrZmqStore
dbr_zmqstore_create(void* ctx, const char* addr, DbrIden seed, DbrPool pool);

DBR_API void
dbr_zmqstore_destroy(DbrZmqStore store);

DBR_API DbrJourn
dbr_zmqstore_journ(DbrZmqStore store);

DBR_API DbrModel
dbr_zmqstore_model(DbrZmqStore store);

/** @} */

#endif // DBR_ZMQSTORE_H
