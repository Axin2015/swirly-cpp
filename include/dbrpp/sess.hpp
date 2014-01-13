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
#ifndef DBRPP_SESS_HPP
#define DBRPP_SESS_HPP

#include <dbr/sess.h>

namespace dbr {

template <class DerivedT>
class ISess : public DbrISess {
    static void
    status_rep(DbrSess sess, DbrIden req_id, int num, const char* msg) noexcept
    {
        static_cast<DerivedT*>(sess)->status_rep(req_id, num, msg);
    }
    static void
    exec_rep(DbrSess sess, DbrIden req_id, DbrExec* exec) noexcept
    {
        static_cast<DerivedT*>(sess)->exec_rep(req_id, *exec);
    }
    static const DbrSessVtbl*
    vtbl() noexcept
    {
        static const DbrSessVtbl VTBL = {
            status_rep,
            exec_rep
        };
        return &VTBL;
    }
public:
    ISess()
        : DbrISess{vtbl()}
    {
    }
};

inline void
status_rep(DbrSess sess, DbrIden req_id, int num, const char* msg)
{
    sess->vtbl->status_rep(sess, req_id, num, msg);
}

inline void
exec_rep(DbrSess sess, DbrIden req_id, DbrExec& exec)
{
    sess->vtbl->exec_rep(sess, req_id, &exec);
}

/** @} */

} // dbr

#endif // DBRPP_SESS_HPP
