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
#ifndef DBR_MODEL_HPP
#define DBR_MODEL_HPP

#include <dbr/except.hpp>
#include <dbr/order.hpp>
#include <dbr/trade.hpp>

#include <dbr/model.h>

#include <cstddef>

namespace dbr {

template <class DerivedT>
class IModel : public DbrIModel {
    static DbrIden
    alloc_id(DbrModel model) noexcept
    {
        return static_cast<DerivedT*>(model)->alloc_id();
    }
    static DbrBool
    begin_trans(DbrModel model) noexcept
    {
        return static_cast<DerivedT*>(model)->begin_trans();
    }
    static DbrBool
    commit_trans(DbrModel model) noexcept
    {
        return static_cast<DerivedT*>(model)->commit_trans();
    }
    static DbrBool
    rollback_trans(DbrModel model) noexcept
    {
        return static_cast<DerivedT*>(model)->rollback_trans();
    }
    static DbrBool
    insert_order(DbrModel model, const DbrOrder* order) noexcept
    {
        return static_cast<DerivedT*>(model)->insert_order(Order(*order));
    }
    static DbrBool
    update_order(DbrModel model, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
                 DbrLots lots, DbrMillis now) noexcept
    {
        return static_cast<DerivedT*>(model)->update_order(id, rev, status, resd, exec, lots, now);
    }
    static DbrBool
    archive_order(DbrModel model, DbrIden id, DbrMillis now) noexcept
    {
        return static_cast<DerivedT*>(model)->archive_order(id, now);
    }
    static DbrBool
    insert_trade(DbrModel model, const DbrTrade* trade) noexcept
    {
        return static_cast<DerivedT*>(model)->insert_trade(Trade(*trade));
    }
    static DbrBool
    archive_trade(DbrModel model, DbrIden id, DbrMillis now) noexcept
    {
        return static_cast<DerivedT*>(model)->archive_trade(id, now);
    }
    static ssize_t
    select_entity(DbrModel model, int type, DbrSlNode** first) noexcept
    {
        return static_cast<DerivedT*>(model)->select_entity(type, *first);
    }
    static DbrSlNode*
    end_entity(DbrModel model) noexcept
    {
        return static_cast<DerivedT*>(model)->end_entity();
    }
    static const DbrModelVtbl*
    vtbl() noexcept
    {
        static const DbrModelVtbl VTBL = {
            alloc_id,
            begin_trans,
            commit_trans,
            rollback_trans,
            insert_order,
            update_order,
            archive_order,
            insert_trade,
            archive_trade,
            select_entity,
            end_entity
        };
        return &VTBL;
    }
public:
    IModel()
        : DbrIModel{ vtbl() }
    {
    }
};

inline DbrIden
alloc_id(DbrModel model)
{
    const auto id = model->vtbl->alloc_id(model);
    if (id < 0)
        throw_exception();
    return id;
}

inline void
begin_trans(DbrModel model)
{
    if (!model->vtbl->begin_trans(model))
        throw_exception();
}

inline void
commit_trans(DbrModel model)
{
    if (!model->vtbl->commit_trans(model))
        throw_exception();
}

inline void
rollback_trans(DbrModel model)
{
    if (!model->vtbl->rollback_trans(model))
        throw_exception();
}

inline void
insert_order(DbrModel model, Order order)
{
    if (!model->vtbl->insert_order(model, static_cast<const DbrOrder*>(order)))
        throw_exception();
}

inline void
update_order(DbrModel model, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
             DbrLots lots, DbrMillis now)
{
    if (!model->vtbl->update_order(model, id, rev, status, resd, exec, lots, now))
        throw_exception();
}

inline void
archive_order(DbrModel model, DbrIden id, DbrMillis now)
{
    if (!model->vtbl->archive_order(model, id, now))
        throw_exception();
}

inline void
insert_trade(DbrModel model, Trade trade)
{
    if (!model->vtbl->insert_trade(model, static_cast<const DbrTrade*>(trade)))
        throw_exception();
}

inline void
archive_trade(DbrModel model, DbrIden id, DbrMillis now)
{
    if (!model->vtbl->archive_trade(model, id, now))
        throw_exception();
}

inline size_t
select_entity(DbrModel model, int type, DbrSlNode*& first)
{
    const auto size = model->vtbl->select_entity(model, type, &first);
    if (size < 0)
        throw_exception();
    return size;
}

inline DbrSlNode*
end_entity(DbrModel model) noexcept
{
    return model->vtbl->end_entity(model);
}

class SqliteModel {
    DbrModel impl_;
public:
    ~SqliteModel() noexcept
    {
        if (impl_)
            dbr_sqlite_destroy(impl_);
    }
    constexpr
    SqliteModel(std::nullptr_t) noexcept
    : impl_(nullptr)
    {
    }
    SqliteModel(DbrPool pool, DbrIden seed, const char* path)
        : impl_(dbr_sqlite_create(pool, seed, path))
    {
        if (!impl_)
            throw_exception();
    }

    // Copy semantics.

    SqliteModel(const SqliteModel&) = delete;

    SqliteModel&
    operator =(const SqliteModel&) = delete;

    // Move semantics.

    SqliteModel(SqliteModel&& rhs) noexcept
    : impl_(nullptr)
    {
        std::swap(impl_, rhs.impl_);
    }
    SqliteModel&
    operator =(SqliteModel&& rhs) noexcept
    {
        if (impl_) {
            dbr_sqlite_destroy(impl_);
            impl_ = nullptr;
        }
        std::swap(impl_, rhs.impl_);
        return *this;
    }
    void
    swap(SqliteModel& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    operator DbrModel() noexcept
    {
        return impl_;
    }
};

} // dbr

#endif // DBR_MODEL_HPP
