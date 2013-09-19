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
#ifndef DBRPP_EXCH_HPP
#define DBRPP_EXCH_HPP

#include <dbrpp/accnt.hpp>
#include <dbrpp/except.hpp>
#include <dbrpp/book.hpp>
#include <dbrpp/order.hpp>
#include <dbrpp/trader.hpp>
#include <dbrpp/result.hpp>

namespace dbr {

template <int TypeN>
class ExchRecs {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrRec Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_model_rec_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_model_rec_entry(const_cast<Node*>(node));
        }
    };
    DbrExch exch_;
public:
    typedef typename Policy::Entry ValueType;
    typedef typename Policy::Entry* Pointer;
    typedef typename Policy::Entry& Reference;
    typedef const typename Policy::Entry* ConstPointer;
    typedef const typename Policy::Entry& ConstReference;

    typedef ForwardIterator<Policy> Iterator;
    typedef ConstForwardIterator<Policy> ConstIterator;

    typedef std::ptrdiff_t DifferenceType;
    typedef size_t SizeType;

    // Standard typedefs.

    typedef ValueType value_type;
    typedef Pointer pointer;
    typedef Reference reference;
    typedef ConstPointer const_pointer;
    typedef ConstReference const_reference;

    typedef Iterator iterator;
    typedef ConstIterator const_iterator;

    typedef DifferenceType difference_type;
    typedef DifferenceType distance_type;
    typedef SizeType size_type;

    explicit
    ExchRecs(DbrExch exch) noexcept
    : exch_(exch)
    {
    }
    void
    swap(ExchRecs& rhs) noexcept
    {
        std::swap(exch_, rhs.exch_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_exch_first_rec(exch_, TypeN, nullptr);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_exch_first_rec(exch_, TypeN, nullptr);
    }
    Iterator
    end() noexcept
    {
        return dbr_exch_end_rec(exch_);
    }
    ConstIterator
    end() const noexcept
    {
        return dbr_exch_end_rec(exch_);
    }
    Iterator
    find(DbrIden id) noexcept
    {
        return dbr_exch_find_rec_id(exch_, TypeN, id);
    }
    ConstIterator
    find(DbrIden id) const noexcept
    {
        return dbr_exch_find_rec_id(exch_, TypeN, id);
    }
    Iterator
    find(const char* mnem) noexcept
    {
        return dbr_exch_find_rec_mnem(exch_, TypeN, mnem);
    }
    ConstIterator
    find(const char* mnem) const noexcept
    {
        return dbr_exch_find_rec_mnem(exch_, TypeN, mnem);
    }

    // Accessor.

    Reference
    front() noexcept
    {
        return *begin();
    }
    ConstReference
    front() const noexcept
    {
        return *begin();
    }
    SizeType
    size() const noexcept
    {
        size_t size;
        dbr_exch_first_rec(exch_, TypeN, &size);
        return size;
    }
    SizeType
    max_size() const noexcept
    {
        return std::numeric_limits<SizeType>::max();
    }
    bool
    empty() const noexcept
    {
        return size() == 0;
    }
};

typedef ExchRecs<DBR_TRADER> ExchTraderRecs;
typedef ExchRecs<DBR_ACCNT> ExchAccntRecs;
typedef ExchRecs<DBR_CONTR> ExchContrRecs;

class Exch {
    DbrExch impl_;
public:
    ~Exch() noexcept
    {
        if (impl_)
            dbr_exch_destroy(impl_);
    }
    constexpr
    Exch(decltype(nullptr)) noexcept
    : impl_(nullptr)
    {
    }
    Exch(DbrJourn journ, DbrModel model, DbrPool pool)
        : impl_(dbr_exch_create(journ, model, pool))
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrExch() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Exch(const Exch&) = delete;

    Exch&
    operator =(const Exch&) = delete;

    // Move semantics.

    Exch(Exch&& rhs) noexcept
    : impl_(nullptr)
    {
        swap(rhs);
    }
    Exch&
    operator =(Exch&& rhs) noexcept
    {
        if (impl_) {
            dbr_exch_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(Exch& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    template <int TypeN>
    ExchRecs<TypeN>
    recs() const noexcept
    {
        return ExchRecs<TypeN>(impl_);
    }
    ExchContrRecs
    crecs() const noexcept
    {
        return ExchContrRecs(impl_);
    }
    ExchTraderRecs
    trecs() const noexcept
    {
        return ExchTraderRecs(impl_);
    }
    ExchAccntRecs
    arecs() const noexcept
    {
        return ExchAccntRecs(impl_);
    }
    BookRef
    book(DbrRec& crec, DbrDate settl_date) const
    {
        DbrBook* const book = dbr_exch_book(impl_, &crec, settl_date);
        if (!book)
            throw_exception();
        return BookRef(*book);
    }
    Trader
    trader(DbrRec& trec) const
    {
        DbrTrader trader = dbr_exch_trader(impl_, &trec);
        if (!trader)
            throw_exception();
        return Trader(trader);
    }
    Accnt
    accnt(DbrRec& arec) const
    {
        DbrAccnt accnt = dbr_exch_accnt(impl_, &arec);
        if (!accnt)
            throw_exception();
        return Accnt(accnt);
    }
    OrderRef
    place(DbrRec& trec, DbrRec& arec, DbrBook& book, const char* ref, int action,
          DbrTicks ticks, DbrLots lots, DbrLots min, DbrFlags flags, Result& result)
    {
        result.reset();
        DbrOrder* const order = dbr_exch_place(impl_, &trec, &arec, &book, ref, action, ticks,
                                               lots, min, flags, result.c_arg());
        if (!order)
            throw_exception();
        return OrderRef(*order);
    }
    OrderRef
    revise(DbrTrader trader, DbrIden id, DbrLots lots)
    {
        DbrOrder* const order = dbr_exch_revise_id(impl_, trader, id, lots);
        if (!order)
            throw_exception();
        return OrderRef(*order);
    }
    OrderRef
    revise(DbrTrader trader, const char* ref, DbrLots lots)
    {
        DbrOrder* const order = dbr_exch_revise_ref(impl_, trader, ref, lots);
        if (!order)
            throw_exception();
        return OrderRef(*order);
    }
    OrderRef
    cancel(DbrTrader trader, DbrIden id)
    {
        DbrOrder* const order = dbr_exch_cancel_id(impl_, trader, id);
        if (!order)
            throw_exception();
        return OrderRef(*order);
    }
    OrderRef
    cancel(DbrTrader trader, const char* ref)
    {
        DbrOrder* const order = dbr_exch_cancel_ref(impl_, trader, ref);
        if (!order)
            throw_exception();
        return OrderRef(*order);
    }
    void
    archive_order(DbrTrader trader, DbrIden id)
    {
        if (!dbr_exch_archive_order(impl_, trader, id))
            throw_exception();
    }
    void
    archive_trade(DbrAccnt accnt, DbrIden id)
    {
        if (!dbr_exch_archive_trade(impl_, accnt, id))
            throw_exception();
    }
};

} // dbr

#endif // DBRPP_EXCH_HPP
