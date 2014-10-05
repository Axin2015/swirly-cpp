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
#ifndef DBRPP_ELM_REST_HPP
#define DBRPP_ELM_REST_HPP

#include <dbrpp/elm/types.hpp>

#include <dbrpp/ash/except.hpp>

#include <dbr/elm/rest.h>

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
    unsigned
    fields() const noexcept
    {
        return impl_.fields;
    }
    DbrIden
    id() const noexcept
    {
        return impl_.id;
    }
    StringRef
    accnt() const noexcept
    {
        return StringRef{impl_.accnt, MaxSize{DBR_MNEM_MAX}};
    }
    StringRef
    giveup() const noexcept
    {
        return StringRef{impl_.giveup, MaxSize{DBR_MNEM_MAX}};
    }
    StringRef
    contr() const noexcept
    {
        return StringRef{impl_.contr, MaxSize{DBR_MNEM_MAX}};
    }
    DbrIsoDate
    settl_date() const noexcept
    {
        return impl_.settl_date;
    }
    StringRef
    ref() const noexcept
    {
        return StringRef{impl_.ref, MaxSize{DBR_REF_MAX}};
    }
    int
    action() const noexcept
    {
        return impl_.action;
    }
    DbrTicks
    ticks() const noexcept
    {
        return impl_.ticks;
    }
    DbrLots
    lots() const noexcept
    {
        return impl_.lots;
    }
    DbrLots
    min_lots() const noexcept
    {
        return impl_.min_lots;
    }
};

/** @} */

} // dbr

#endif // DBRPP_ELM_REST_HPP
