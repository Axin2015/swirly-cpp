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
#ifndef DBR_CTX_HPP
#define DBR_CTX_HPP

#include <dbr/except.hpp>
#include <dbr/order.hpp>

namespace dbr {

class Ctx {
    DbrCtx impl_;
public:
    ~Ctx() noexcept
    {
        if (impl_)
            dbr_ctx_destroy(impl_);
    }
    constexpr
    Ctx(std::nullptr_t) noexcept
    : impl_(nullptr)
    {
    }
    Ctx(DbrPool pool, DbrModel model)
        : impl_(dbr_ctx_create(pool, model))
    {
        if (!impl_)
            throw_exception();
    }

    // Copy semantics.

    Ctx(const Ctx&) = delete;

    Ctx&
    operator =(const Ctx&) = delete;

    // Move semantics.

    Ctx(Ctx&& rhs) noexcept
    : impl_(nullptr)
    {
        std::swap(impl_, rhs.impl_);
    }
    Ctx&
    operator =(Ctx&& rhs) noexcept
    {
        if (impl_) {
            dbr_ctx_destroy(impl_);
            impl_ = nullptr;
        }
        std::swap(impl_, rhs.impl_);
        return *this;
    }
    void
    swap(Ctx& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    operator DbrCtx() noexcept
    {
        return impl_;
    }
    InstrRecs
    irecs() const noexcept
    {
        return InstrRecs(impl_);
    }
    MarketRecs
    mrecs() const noexcept
    {
        return MarketRecs(impl_);
    }
    TraderRecs
    trecs() const noexcept
    {
        return TraderRecs(impl_);
    }
    AccntRecs
    arecs() const noexcept
    {
        return AccntRecs(impl_);
    }
    DbrMarket
    market(DbrRec& mrec) const
    {
        DbrMarket market = dbr_ctx_market(impl_, &mrec);
        if (!market)
            throw_exception();
        return market;
    }
    DbrTrader
    trader(DbrRec& trec) const
    {
        DbrTrader trader = dbr_ctx_trader(impl_, &trec);
        if (!trader)
            throw_exception();
        return trader;
    }
    DbrAccnt
    accnt(DbrRec& arec) const
    {
        DbrAccnt accnt = dbr_ctx_accnt(impl_, &arec);
        if (!accnt)
            throw_exception();
        return accnt;
    }
    Order
    submit(DbrRec& trec, DbrRec& arec, const char* ref, DbrRec& mrec, int action,
           DbrTicks ticks, DbrLots lots, DbrLots min, DbrFlags flags, DbrTrans& trans)
    {
        DbrOrder* const order = dbr_ctx_submit(impl_, &trec, &arec, ref, &mrec, action,
                                               ticks, lots, min, flags, &trans);
        if (!order)
            throw_exception();
        return Order(*order);
    }
    Order
    revise(DbrTrader trader, DbrIden id, DbrLots lots)
    {
        DbrOrder* const order = dbr_ctx_revise_id(impl_, trader, id, lots);
        if (!order)
            throw_exception();
        return Order(*order);
    }
    Order
    revise(DbrTrader trader, const char* ref, DbrLots lots)
    {
        DbrOrder* const order = dbr_ctx_revise_ref(impl_, trader, ref, lots);
        if (!order)
            throw_exception();
        return Order(*order);
    }
    Order
    cancel(DbrTrader trader, DbrIden id)
    {
        DbrOrder* const order = dbr_ctx_cancel_id(impl_, trader, id);
        if (!order)
            throw_exception();
        return Order(*order);
    }
    Order
    cancel(DbrTrader trader, const char* ref)
    {
        DbrOrder* const order = dbr_ctx_cancel_ref(impl_, trader, ref);
        if (!order)
            throw_exception();
        return Order(*order);
    }
    void
    archive_order(DbrTrader trader, DbrIden id)
    {
        if (!dbr_ctx_archive_order(impl_, trader, id))
            throw_exception();
    }
    void
    archive_trade(DbrAccnt accnt, DbrIden id)
    {
        if (!dbr_ctx_archive_trade(impl_, accnt, id))
            throw_exception();
    }
    void
    free_matches(struct DbrSlNode* first) noexcept
    {
        dbr_ctx_free_matches(impl_, first);
    }
};

} // dbr

#endif // DBR_CTX_HPP
