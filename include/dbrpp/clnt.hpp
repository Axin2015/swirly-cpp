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
#ifndef DBRPP_CLNT_HPP
#define DBRPP_CLNT_HPP

#include <dbrpp/accnt.hpp>
#include <dbrpp/except.hpp>
#include <dbrpp/order.hpp>
#include <dbrpp/slnode.hpp>
#include <dbrpp/trader.hpp>

#include <dbr/clnt.h>

namespace dbr {

template <int TypeN>
class ClntRecs {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrRec Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_entity_rec_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_entity_rec_entry(const_cast<Node*>(node));
        }
    };
    DbrClnt clnt_;
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
    ClntRecs(DbrClnt clnt) noexcept
        : clnt_{clnt}
    {
    }
    void
    swap(ClntRecs& rhs) noexcept
    {
        std::swap(clnt_, rhs.clnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_clnt_first_rec(clnt_, TypeN, nullptr);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_clnt_first_rec(clnt_, TypeN, nullptr);
    }
    Iterator
    end() noexcept
    {
        return DBR_CLNT_END_REC;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_CLNT_END_REC;
    }
    Iterator
    find(DbrIden id) noexcept
    {
        return dbr_clnt_find_rec_id(clnt_, TypeN, id);
    }
    ConstIterator
    find(DbrIden id) const noexcept
    {
        return dbr_clnt_find_rec_id(clnt_, TypeN, id);
    }
    Iterator
    find(const char* mnem) noexcept
    {
        return dbr_clnt_find_rec_mnem(clnt_, TypeN, mnem);
    }
    ConstIterator
    find(const char* mnem) const noexcept
    {
        return dbr_clnt_find_rec_mnem(clnt_, TypeN, mnem);
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
        dbr_clnt_first_rec(clnt_, TypeN, &size);
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

typedef ClntRecs<DBR_TRADER> ClntTraderRecs;
typedef ClntRecs<DBR_ACCNT> ClntAccntRecs;
typedef ClntRecs<DBR_CONTR> ClntContrRecs;

class Clnt {
    DbrClnt impl_;
public:
    ~Clnt() noexcept
    {
        if (impl_)
            dbr_clnt_destroy(impl_);
    }
    constexpr
    Clnt(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    Clnt(void* ctx, const char* addr, const char* trader, DbrIden seed, DbrPool pool)
        : impl_{dbr_clnt_create(ctx, addr, trader, seed, pool)}
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrClnt() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Clnt(const Clnt&) = delete;

    Clnt&
    operator =(const Clnt&) = delete;

    // Move semantics.

    Clnt(Clnt&& rhs) noexcept
        : impl_{nullptr}
    {
        swap(rhs);
    }
    Clnt&
    operator =(Clnt&& rhs) noexcept
    {
        if (impl_) {
            dbr_clnt_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(Clnt& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    template <int TypeN>
    ClntRecs<TypeN>
    recs() const noexcept
    {
        return ClntRecs<TypeN>{impl_};
    }
    ClntContrRecs
    crecs() const noexcept
    {
        return ClntContrRecs{impl_};
    }
    ClntTraderRecs
    trecs() const noexcept
    {
        return ClntTraderRecs{impl_};
    }
    ClntAccntRecs
    arecs() const noexcept
    {
        return ClntAccntRecs{impl_};
    }
    Trader
    trader() const noexcept
    {
        return Trader{dbr_clnt_trader(impl_)};
    }
    Accnt
    accnt(DbrRec& arec) const
    {
        DbrAccnt accnt = dbr_clnt_accnt(impl_, &arec);
        if (!accnt)
            throw_exception();
        return Accnt{accnt};
    }
    OrderRef
    place(const char* accnt, const char* contr, DbrDate settl_date, const char* ref, int action,
          DbrTicks ticks, DbrLots lots, DbrLots min_lots)
    {
        DbrOrder* const order = dbr_clnt_place(impl_, accnt, contr, settl_date, ref, action,
                                               ticks, lots, min_lots);
        if (!order)
            throw_exception();
        return OrderRef{*order};
    }
    OrderRef
    revise(DbrIden id, DbrLots lots)
    {
        DbrOrder* const order = dbr_clnt_revise_id(impl_, id, lots);
        if (!order)
            throw_exception();
        return OrderRef{*order};
    }
    OrderRef
    revise(const char* ref, DbrLots lots)
    {
        DbrOrder* const order = dbr_clnt_revise_ref(impl_, ref, lots);
        if (!order)
            throw_exception();
        return OrderRef{*order};
    }
    OrderRef
    cancel(DbrIden id)
    {
        DbrOrder* const order = dbr_clnt_cancel_id(impl_, id);
        if (!order)
            throw_exception();
        return OrderRef{*order};
    }
    OrderRef
    cancel(const char* ref)
    {
        DbrOrder* const order = dbr_clnt_cancel_ref(impl_, ref);
        if (!order)
            throw_exception();
        return OrderRef{*order};
    }
    void
    archive_order(DbrIden id)
    {
        if (!dbr_clnt_archive_order(impl_, id))
            throw_exception();
    }
    void
    archive_trade(DbrIden id)
    {
        if (!dbr_clnt_archive_trade(impl_, id))
            throw_exception();
    }
};

} // dbr

#endif // DBRPP_CLNT_HPP
