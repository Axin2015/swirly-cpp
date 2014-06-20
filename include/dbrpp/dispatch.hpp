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
#ifndef DBRPP_DISPATCH_HPP
#define DBRPP_DISPATCH_HPP

#include <dbrpp/except.hpp>

#include <dbr/fig/dispatch.h>

namespace dbr {

/**
 * @addtogroup Clnt
 * @{
 */

inline void
dispatch(DbrClnt clnt, DbrMillis ms, DbrHandler handler)
{
    if (!dbr_clnt_dispatch(clnt, ms, handler))
        throw_exception();
}

/** @} */

} // dbr

#endif // DBRPP_DISPATCH_HPP
