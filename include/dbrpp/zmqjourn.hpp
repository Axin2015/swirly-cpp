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
#ifndef DBRPP_ZMQJOURN_HPP
#define DBRPP_ZMQJOURN_HPP

#include <dbrpp/journ.hpp>

#include <dbr/zmqjourn.h>

namespace dbr {

/**
 * @addtogroup Zmq
 * @{
 */

inline JournPtr
zmqjourn_create(void* zctx, size_t capacity, DbrJourn (*factory)(void*), void* arg)
{
    JournPtr ptr{dbr_zmqjourn_create(zctx, capacity, factory, arg)};
    if (!ptr)
        throw_exception();
    return ptr;
}

/** @} */

} // dbr

#endif // DBRPP_ZMQJOURN_HPP
