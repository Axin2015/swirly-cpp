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
#ifndef DBRPP_JOURN_HPP
#define DBRPP_JOURN_HPP

#include <dbrpp/except.hpp>
#include <dbrpp/exec.hpp>
#include <dbrpp/order.hpp>

#include <dbr/journ.h>

namespace dbr {

template <class DerivedT>
class IJourn : public DbrIJourn {
    static DbrIden
    alloc_id(DbrJourn journ) noexcept
    {
        return static_cast<DerivedT*>(journ)->alloc_id();
    }
    static DbrBool
    insert_execs(DbrJourn journ, DbrSlNode* first) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_execs(first);
    }
    static DbrBool
    insert_exec(DbrJourn journ, struct DbrExec* exec) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_exec(*exec);
    }
    static DbrBool
    update_exec(DbrJourn journ, DbrIden id, DbrMillis modified) noexcept
    {
        return static_cast<DerivedT*>(journ)->update_exec(id, modified);
    }
    static const DbrJournVtbl*
    vtbl() noexcept
    {
        static const DbrJournVtbl VTBL = {
            alloc_id,
            insert_execs,
            insert_exec,
            update_exec
        };
        return &VTBL;
    }
public:
    IJourn()
        : DbrIJourn{vtbl()}
    {
    }
};

inline DbrIden
alloc_id(DbrJourn journ)
{
    const auto id = journ->vtbl->alloc_id(journ);
    if (id < 0)
        throw_exception();
    return id;
}

inline void
insert_execs(DbrJourn journ, DbrSlNode* first)
{
    if (!journ->vtbl->insert_execs(journ, first))
        throw_exception();
}

inline void
insert_exec(DbrJourn journ, DbrExec& exec)
{
    if (!journ->vtbl->insert_exec(journ, &exec))
        throw_exception();
}

inline void
update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    if (!journ->vtbl->update_exec(journ, id, modified))
        throw_exception();
}

} // dbr

#endif // DBRPP_JOURN_HPP
