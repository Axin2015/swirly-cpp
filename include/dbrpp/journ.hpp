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
#ifndef DBRPP_JOURN_HPP
#define DBRPP_JOURN_HPP

#include <dbrpp/except.hpp>
#include <dbrpp/exec.hpp>
#include <dbrpp/order.hpp>

#include <dbr/journ.h>

namespace dbr {

/**
 * @addtogroup Journ
 * @{
 */

template <class DerivedT>
class IJourn : public DbrIJourn {
    static void
    destroy(DbrJourn journ) noexcept
    {
        return static_cast<DerivedT*>(journ)->destroy();
    }
    static DbrBool
    insert_exec_list(DbrJourn journ, DbrSlNode* first, DbrBool enriched) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_exec_list(first, enriched);
    }
    static DbrBool
    insert_exec(DbrJourn journ, DbrExec* exec, DbrBool enriched) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_exec(*exec, enriched);
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
            destroy,
            insert_exec_list,
            insert_exec,
            update_exec
        };
        return &VTBL;
    }
protected:
    ~IJourn() noexcept
    {
    }
public:
    IJourn() noexcept
        : DbrIJourn{vtbl()}
    {
    }
};

inline void
destroy(DbrJourn journ)
{
    journ->vtbl->destroy(journ);
}

inline void
insert_exec_list(DbrJourn journ, DbrSlNode* first, DbrBool enriched)
{
    if (!journ->vtbl->insert_exec_list(journ, first, enriched))
        throw_exception();
}

inline void
insert_exec(DbrJourn journ, DbrExec& exec, DbrBool enriched)
{
    if (!journ->vtbl->insert_exec(journ, &exec, enriched))
        throw_exception();
}

inline void
update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    if (!journ->vtbl->update_exec(journ, id, modified))
        throw_exception();
}

/** @} */

} // dbr

#endif // DBRPP_JOURN_HPP
