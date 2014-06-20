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
#ifndef DBRPP_ASH_SHLEX_HPP
#define DBRPP_ASH_SHLEX_HPP

#include <dbrpp/ash/except.hpp>

#include <dbr/ash/shlex.h>

namespace dbr {

/**
 * @addtogroup Shlex
 * @{
 */

template <typename FnT>
class Shlex {
    FnT fn_;
    DbrShlex impl_;
    static void
    cb(void* ctx, const char* tok, size_t len) noexcept
    {
        static_cast<Shlex*>(ctx)->fn_(tok, len);
    }
public:
    explicit
    Shlex(FnT fn) noexcept
        : fn_{fn}
    {
        dbr_shlex_init(&impl_, cb, this);
    }
    void
    reset() noexcept
    {
        dbr_shlex_reset(&impl_);
    }
    void
    exec(const char* buf, size_t size)
    {
        if (!dbr_shlex_exec(&impl_, buf, size))
            throw_exception();
    }
};

template <typename FnT>
Shlex<FnT>
make_shlex(FnT fn)
{
    return Shlex<FnT>{fn};
}

/** @} */

} // dbr

#endif // DBRPP_ASH_SHLEX_HPP
