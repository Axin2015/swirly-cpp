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
#ifndef DBRPP_REST_HPP
#define DBRPP_REST_HPP

#include <dbrpp/except.hpp>

#include <dbr/rest.h>

namespace dbr {

/**
 * @addtogroup Rest
 * @{
 */

class Rest {
    DbrRest impl_;
public:
    Rest() noexcept
    {
        dbr_rest_init(&impl_);
    }
    void
    reset() noexcept
    {
        dbr_rest_init(&impl_);
    }
    bool
    rurl(const char* buf, size_t size)
    {
        const int ret = dbr_rest_rurl(&impl_, buf, size);
        if (ret < 0)
            throw_exception();
        return ret;
    }
    bool
    json(const char* buf, size_t size)
    {
        const int ret = dbr_rest_json(&impl_, buf, size);
        if (ret < 0)
            throw_exception();
        return ret;
    }
};

/** @} */

} // dbr

#endif // DBRPP_REST_HPP
