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
    begin_trans(DbrJourn journ) noexcept
    {
        return static_cast<DerivedT*>(journ)->begin_trans();
    }
    static DbrBool
    commit_trans(DbrJourn journ) noexcept
    {
        return static_cast<DerivedT*>(journ)->commit_trans();
    }
    static DbrBool
    rollback_trans(DbrJourn journ) noexcept
    {
        return static_cast<DerivedT*>(journ)->rollback_trans();
    }
    static DbrBool
    insert_exec(DbrJourn journ, const DbrExec* exec) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_exec(*exec);
    }
    static DbrBool
    insert_stmt(DbrJourn journ, const DbrStmt* stmt) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_stmt(*stmt);
    }
    static DbrBool
    archive_trade(DbrJourn journ, DbrIden id, DbrMillis now) noexcept
    {
        return static_cast<DerivedT*>(journ)->archive_trade(id, now);
    }
    static const DbrJournVtbl*
    vtbl() noexcept
    {
        static const DbrJournVtbl VTBL = {
            alloc_id,
            begin_trans,
            commit_trans,
            rollback_trans,
            insert_exec,
            insert_stmt,
            archive_trade
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
begin_trans(DbrJourn journ)
{
    if (!journ->vtbl->begin_trans(journ))
        throw_exception();
}

inline void
commit_trans(DbrJourn journ)
{
    if (!journ->vtbl->commit_trans(journ))
        throw_exception();
}

inline void
rollback_trans(DbrJourn journ)
{
    if (!journ->vtbl->rollback_trans(journ))
        throw_exception();
}

inline void
insert_exec(DbrJourn journ, const DbrExec& exec)
{
    if (!journ->vtbl->insert_exec(journ, &exec))
        throw_exception();
}

inline void
insert_stmt(DbrJourn journ, const DbrStmt& stmt)
{
    if (!journ->vtbl->insert_stmt(journ, &stmt))
        throw_exception();
}

inline void
archive_trade(DbrJourn journ, DbrIden id, DbrMillis now)
{
    if (!journ->vtbl->archive_trade(journ, id, now))
        throw_exception();
}

} // dbr

#endif // DBRPP_JOURN_HPP
