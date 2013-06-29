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
#ifndef DBR_JOURN_HPP
#define DBR_JOURN_HPP

#include <dbr/except.hpp>
#include <dbr/order.hpp>
#include <dbr/trade.hpp>

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
    insert_order(DbrJourn journ, DbrOrder* order) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_order(Order(*order));
    }
    static DbrBool
    update_order(DbrJourn journ, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
                 DbrLots lots, DbrMillis now) noexcept
    {
        return static_cast<DerivedT*>(journ)->update_order(id, rev, status, resd, exec, lots, now);
    }
    static DbrBool
    archive_order(DbrJourn journ, DbrIden id, DbrMillis now) noexcept
    {
        return static_cast<DerivedT*>(journ)->archive_order(id, now);
    }
    static DbrBool
    insert_trade(DbrJourn journ, DbrTrade* trade) noexcept
    {
        return static_cast<DerivedT*>(journ)->insert_trade(Trade(*trade));
    }
    static DbrBool
    archive_trade(DbrJourn journ, DbrIden id, DbrMillis now) noexcept
    {
        return static_cast<DerivedT*>(journ)->archive_trade(id, now);
    }
    static ssize_t
    select_entity(DbrJourn journ, int type, DbrSlNode** first) noexcept
    {
        return static_cast<DerivedT*>(journ)->select_entity(type, *first);
    }
    static DbrSlNode*
    end_entity(DbrJourn journ) noexcept
    {
        return static_cast<DerivedT*>(journ)->end_entity();
    }
    static const DbrJournVtbl*
    vtbl() noexcept
    {
        static const DbrJournVtbl VTBL = {
            alloc_id,
            begin_trans,
            commit_trans,
            rollback_trans,
            insert_order,
            update_order,
            archive_order,
            insert_trade,
            archive_trade
        };
        return &VTBL;
    }
public:
    IJourn()
        : DbrIJourn{ vtbl() }
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
insert_order(DbrJourn journ, Order order)
{
    if (!journ->vtbl->insert_order(journ, order.c_arg()))
        throw_exception();
}

inline void
update_order(DbrJourn journ, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
             DbrLots lots, DbrMillis now)
{
    if (!journ->vtbl->update_order(journ, id, rev, status, resd, exec, lots, now))
        throw_exception();
}

inline void
archive_order(DbrJourn journ, DbrIden id, DbrMillis now)
{
    if (!journ->vtbl->archive_order(journ, id, now))
        throw_exception();
}

inline void
insert_trade(DbrJourn journ, Trade trade)
{
    if (!journ->vtbl->insert_trade(journ, trade.c_arg()))
        throw_exception();
}

inline void
archive_trade(DbrJourn journ, DbrIden id, DbrMillis now)
{
    if (!journ->vtbl->archive_trade(journ, id, now))
        throw_exception();
}

} // dbr

#endif // DBR_JOURN_HPP
