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
#ifndef DBRPP_SERV_HPP
#define DBRPP_SERV_HPP

#include <dbrpp/accnt.hpp>
#include <dbrpp/book.hpp>
#include <dbrpp/except.hpp>
#include <dbrpp/order.hpp>
#include <dbrpp/slnode.hpp>
#include <dbrpp/trader.hpp>

#include <dbr/serv.h>

namespace dbr {

template <int TypeN>
class ServRecs {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrRec Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_serv_rec_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_serv_rec_entry(const_cast<Node*>(node));
        }
    };
    DbrServ serv_;
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
    ServRecs(DbrServ serv) noexcept
        : serv_{serv}
    {
    }
    void
    swap(ServRecs& rhs) noexcept
    {
        std::swap(serv_, rhs.serv_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_serv_first_rec(serv_, TypeN, nullptr);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_serv_first_rec(serv_, TypeN, nullptr);
    }
    Iterator
    end() noexcept
    {
        return DBR_SERV_END_REC;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_SERV_END_REC;
    }
    Iterator
    find(DbrIden id) noexcept
    {
        return dbr_serv_find_rec_id(serv_, TypeN, id);
    }
    ConstIterator
    find(DbrIden id) const noexcept
    {
        return dbr_serv_find_rec_id(serv_, TypeN, id);
    }
    Iterator
    find(const char* mnem) noexcept
    {
        return dbr_serv_find_rec_mnem(serv_, TypeN, mnem);
    }
    ConstIterator
    find(const char* mnem) const noexcept
    {
        return dbr_serv_find_rec_mnem(serv_, TypeN, mnem);
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
        dbr_serv_first_rec(serv_, TypeN, &size);
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
        return dbr_serv_empty_rec(serv_, TypeN) == DBR_TRUE;
    }
};

typedef ServRecs<DBR_TRADER> ServTraderRecs;
typedef ServRecs<DBR_ACCNT> ServAccntRecs;
typedef ServRecs<DBR_CONTR> ServContrRecs;

class ServBooks {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrBook Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_serv_book_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_serv_book_entry(const_cast<Node*>(node));
        }
    };
    DbrServ serv_;
public:
    typedef Policy::Entry ValueType;
    typedef Policy::Entry* Pointer;
    typedef Policy::Entry& Reference;
    typedef const Policy::Entry* ConstPointer;
    typedef const Policy::Entry& ConstReference;

    typedef BiDirectionalIterator<Policy> Iterator;
    typedef ConstBiDirectionalIterator<Policy> ConstIterator;
    typedef ReverseBiDirectionalIterator<Policy> ReverseIterator;
    typedef ConstReverseBiDirectionalIterator<Policy> ConstReverseIterator;

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
    typedef ReverseIterator reverse_iterator;
    typedef ConstReverseIterator const_reverse_iterator;

    typedef DifferenceType difference_type;
    typedef DifferenceType distance_type;
    typedef SizeType size_type;

    explicit
    ServBooks(DbrServ serv) noexcept
    : serv_{serv}
    {
    }
    void
    swap(ServBooks& rhs) noexcept
    {
        std::swap(serv_, rhs.serv_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_serv_first_book(serv_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_serv_first_book(serv_);
    }
    Iterator
    end() noexcept
    {
        return DBR_SERV_END_BOOK;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_SERV_END_BOOK;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_serv_last_book(serv_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_serv_last_book(serv_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_SERV_END_BOOK;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_SERV_END_BOOK;
    }

    // Find.

    Iterator
    find(DbrIden cid, DbrDate settl_date) noexcept
    {
        return dbr_serv_find_book(serv_, cid, settl_date);
    }
    ConstIterator
    find(DbrIden cid, DbrDate settl_date) const noexcept
    {
        return dbr_serv_find_book(serv_, cid, settl_date);
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
    Reference
    back() noexcept
    {
        return *rbegin();
    }
    ConstReference
    back() const noexcept
    {
        return *rbegin();
    }
    SizeType
    size() const noexcept
    {
        return std::distance(begin(), end());
    }
    SizeType
    max_size() const noexcept
    {
        return std::numeric_limits<SizeType>::max();
    }
    bool
    empty() const noexcept
    {
        return dbr_serv_empty_book(serv_);
    }
};

class ServExecs {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrExec Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_serv_exec_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_serv_exec_entry(const_cast<Node*>(node));
        }
    };
    DbrServ serv_;
public:
    typedef Policy::Entry ValueType;
    typedef Policy::Entry* Pointer;
    typedef Policy::Entry& Reference;
    typedef const Policy::Entry* ConstPointer;
    typedef const Policy::Entry& ConstReference;

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
    ServExecs(DbrServ serv) noexcept
        : serv_{serv}
    {
    }
    void
    swap(ServExecs& rhs) noexcept
    {
        std::swap(serv_, rhs.serv_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_serv_first_exec(serv_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_serv_first_exec(serv_);
    }
    Iterator
    end() noexcept
    {
        return nullptr;
    }
    ConstIterator
    end() const noexcept
    {
        return nullptr;
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
        return std::distance(begin(), end());
    }
    SizeType
    max_size() const noexcept
    {
        return std::numeric_limits<SizeType>::max();
    }
    bool
    empty() const noexcept
    {
        return dbr_serv_empty_exec(serv_);
    }
};

class ServPosnups {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrPosn Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_serv_posnup_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_serv_posnup_entry(const_cast<Node*>(node));
        }
    };
    DbrServ serv_;
public:
    typedef Policy::Entry ValueType;
    typedef Policy::Entry* Pointer;
    typedef Policy::Entry& Reference;
    typedef const Policy::Entry* ConstPointer;
    typedef const Policy::Entry& ConstReference;

    typedef BiDirectionalIterator<Policy> Iterator;
    typedef ConstBiDirectionalIterator<Policy> ConstIterator;
    typedef ReverseBiDirectionalIterator<Policy> ReverseIterator;
    typedef ConstReverseBiDirectionalIterator<Policy> ConstReverseIterator;

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
    typedef ReverseIterator reverse_iterator;
    typedef ConstReverseIterator const_reverse_iterator;

    typedef DifferenceType difference_type;
    typedef DifferenceType distance_type;
    typedef SizeType size_type;

    explicit
    ServPosnups(DbrServ serv) noexcept
    : serv_{serv}
    {
    }
    void
    swap(ServPosnups& rhs) noexcept
    {
        std::swap(serv_, rhs.serv_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_serv_first_posnup(serv_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_serv_first_posnup(serv_);
    }
    Iterator
    end() noexcept
    {
        return DBR_SERV_END_POSNUP;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_SERV_END_POSNUP;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_serv_last_posnup(serv_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_serv_last_posnup(serv_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_SERV_END_POSNUP;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_SERV_END_POSNUP;
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
    Reference
    back() noexcept
    {
        return *rbegin();
    }
    ConstReference
    back() const noexcept
    {
        return *rbegin();
    }
    SizeType
    size() const noexcept
    {
        return std::distance(begin(), end());
    }
    SizeType
    max_size() const noexcept
    {
        return std::numeric_limits<SizeType>::max();
    }
    bool
    empty() const noexcept
    {
        return dbr_serv_empty_posnup(serv_);
    }
};

class ServBookups {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrBook Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_serv_bookup_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_serv_bookup_entry(const_cast<Node*>(node));
        }
    };
    DbrServ serv_;
public:
    typedef Policy::Entry ValueType;
    typedef Policy::Entry* Pointer;
    typedef Policy::Entry& Reference;
    typedef const Policy::Entry* ConstPointer;
    typedef const Policy::Entry& ConstReference;

    typedef BiDirectionalIterator<Policy> Iterator;
    typedef ConstBiDirectionalIterator<Policy> ConstIterator;
    typedef ReverseBiDirectionalIterator<Policy> ReverseIterator;
    typedef ConstReverseBiDirectionalIterator<Policy> ConstReverseIterator;

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
    typedef ReverseIterator reverse_iterator;
    typedef ConstReverseIterator const_reverse_iterator;

    typedef DifferenceType difference_type;
    typedef DifferenceType distance_type;
    typedef SizeType size_type;

    explicit
    ServBookups(DbrServ serv) noexcept
    : serv_{serv}
    {
    }
    void
    swap(ServBookups& rhs) noexcept
    {
        std::swap(serv_, rhs.serv_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_serv_first_bookup(serv_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_serv_first_bookup(serv_);
    }
    Iterator
    end() noexcept
    {
        return DBR_SERV_END_BOOKUP;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_SERV_END_BOOKUP;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_serv_last_bookup(serv_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_serv_last_bookup(serv_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_SERV_END_BOOKUP;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_SERV_END_BOOKUP;
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
    Reference
    back() noexcept
    {
        return *rbegin();
    }
    ConstReference
    back() const noexcept
    {
        return *rbegin();
    }
    SizeType
    size() const noexcept
    {
        return std::distance(begin(), end());
    }
    SizeType
    max_size() const noexcept
    {
        return std::numeric_limits<SizeType>::max();
    }
    bool
    empty() const noexcept
    {
        return dbr_serv_empty_bookup(serv_);
    }
};

class Serv {
    DbrServ impl_;
public:
    ~Serv() noexcept
    {
        if (impl_)
            dbr_serv_destroy(impl_);
    }
    constexpr
    Serv(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    Serv(DbrJourn journ, DbrModel model, DbrPool pool)
        : impl_{dbr_serv_create(journ, model, pool)}
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrServ() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Serv(const Serv&) = delete;

    Serv&
    operator =(const Serv&) = delete;

    // Move semantics.

    Serv(Serv&& rhs) noexcept
        : impl_{nullptr}
    {
        swap(rhs);
    }
    Serv&
    operator =(Serv&& rhs) noexcept
    {
        if (impl_) {
            dbr_serv_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(Serv& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    template <int TypeN>
    ServRecs<TypeN>
    recs() const noexcept
    {
        return ServRecs<TypeN>{impl_};
    }
    ServContrRecs
    crecs() const noexcept
    {
        return ServContrRecs{impl_};
    }
    ServTraderRecs
    trecs() const noexcept
    {
        return ServTraderRecs{impl_};
    }
    ServAccntRecs
    arecs() const noexcept
    {
        return ServAccntRecs{impl_};
    }
    ServBooks
    books() const noexcept
    {
        return ServBooks{impl_};
    }
    BookRef
    book(DbrRec& crec, DbrDate settl_date) const
    {
        DbrBook* const book = dbr_serv_book(impl_, &crec, settl_date);
        if (!book)
            throw_exception();
        return BookRef{*book};
    }
    Trader
    trader(DbrRec& trec) const
    {
        DbrTrader trader = dbr_serv_trader(impl_, &trec);
        if (!trader)
            throw_exception();
        return Trader{trader};
    }
    Accnt
    accnt(DbrRec& arec) const
    {
        DbrAccnt accnt = dbr_serv_accnt(impl_, &arec);
        if (!accnt)
            throw_exception();
        return Accnt{accnt};
    }
    OrderRef
    place(DbrTrader trader, DbrAccnt accnt, DbrBook& book, const char* ref, int action,
          DbrTicks ticks, DbrLots lots, DbrLots min_lots)
    {
        DbrOrder* const order = dbr_serv_place(impl_, trader, accnt, &book, ref, action, ticks,
                                               lots, min_lots);
        if (!order)
            throw_exception();
        return OrderRef{*order};
    }
    OrderRef
    revise(DbrTrader trader, DbrIden id, DbrLots lots)
    {
        DbrOrder* const order = dbr_serv_revise_id(impl_, trader, id, lots);
        if (!order)
            throw_exception();
        return OrderRef{*order};
    }
    OrderRef
    revise(DbrTrader trader, const char* ref, DbrLots lots)
    {
        DbrOrder* const order = dbr_serv_revise_ref(impl_, trader, ref, lots);
        if (!order)
            throw_exception();
        return OrderRef{*order};
    }
    OrderRef
    cancel(DbrTrader trader, DbrIden id)
    {
        DbrOrder* const order = dbr_serv_cancel_id(impl_, trader, id);
        if (!order)
            throw_exception();
        return OrderRef{*order};
    }
    OrderRef
    cancel(DbrTrader trader, const char* ref)
    {
        DbrOrder* const order = dbr_serv_cancel_ref(impl_, trader, ref);
        if (!order)
            throw_exception();
        return OrderRef{*order};
    }
    void
    ack_trade(DbrTrader trader, DbrIden id)
    {
        if (!dbr_serv_ack_trade(impl_, trader, id))
            throw_exception();
    }
    void
    clear() noexcept
    {
        dbr_serv_clear(impl_);
    }
    ServExecs
    execs() const noexcept
    {
        return ServExecs{impl_};
    }
    ServPosnups
    posnups() const noexcept
    {
        return ServPosnups{impl_};
    }
    ServBookups
    bookups() const noexcept
    {
        return ServBookups{impl_};
    }
};

} // dbr

#endif // DBRPP_SERV_HPP
