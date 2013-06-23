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
    begin(DbrModel model) noexcept
    {
        return static_cast<DerivedT*>(model)->begin();
    }
    static DbrBool
    commit(DbrModel model) noexcept
    {
        return static_cast<DerivedT*>(model)->commit();
    }
    static DbrBool
    rollback(DbrModel model) noexcept
    {
        return static_cast<DerivedT*>(model)->rollback();
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
    select(DbrModel model, int type, DbrSlNode** first) noexcept
    {
        return static_cast<DerivedT*>(model)->select(type, *first);
    }
    static DbrSlNode*
    end(DbrModel model) noexcept
    {
        return static_cast<DerivedT*>(model)->end();
    }
    static const DbrModelVtbl*
    vtbl() noexcept
    {
        static const DbrModelVtbl VTBL = {
            alloc_id,
            begin,
            commit,
            rollback,
            insert_order,
            update_order,
            archive_order,
            insert_trade,
            archive_trade,
            select,
            end
        };
        return &VTBL;
    }
public:
    IModel()
        : DbrIModel{ vtbl() }
    {
    }
};

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
