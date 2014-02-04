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
#ifndef DBRPP_CLNT_HPP
#define DBRPP_CLNT_HPP

#include <dbrpp/accnt.hpp>
#include <dbrpp/except.hpp>
#include <dbrpp/order.hpp>
#include <dbrpp/slnode.hpp>
#include <dbrpp/trader.hpp>

#include <dbr/clnt.h>

namespace dbr {

/**
 * @addtogroup ClntRec
 * @{
 */

template <int TypeN>
class ClntRecs {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrRec Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_clnt_rec_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_clnt_rec_entry(const_cast<Node*>(node));
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
        return dbr_clnt_empty_rec(clnt_, TypeN) == DBR_TRUE;
    }
};

typedef ClntRecs<DBR_ENTITY_TRADER> ClntTraderRecs;
typedef ClntRecs<DBR_ENTITY_ACCNT> ClntAccntRecs;
typedef ClntRecs<DBR_ENTITY_CONTR> ClntContrRecs;

/** @} */

/**
 * @addtogroup ClntExec
 * @{
 */

class ClntExecs {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrExec Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_clnt_exec_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_clnt_exec_entry(const_cast<Node*>(node));
        }
    };
    DbrClnt clnt_;
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
    ClntExecs(DbrClnt clnt) noexcept
        : clnt_{clnt}
    {
    }
    void
    swap(ClntExecs& rhs) noexcept
    {
        std::swap(clnt_, rhs.clnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_clnt_first_exec(clnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_clnt_first_exec(clnt_);
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
        return dbr_clnt_empty_exec(clnt_);
    }
};

/** @} */

/**
 * @addtogroup ClntPosn
 * @{
 */

class ClntPosnups {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrPosn Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_clnt_posnup_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_clnt_posnup_entry(const_cast<Node*>(node));
        }
    };
    DbrClnt clnt_;
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
    ClntPosnups(DbrClnt clnt) noexcept
    : clnt_{clnt}
    {
    }
    void
    swap(ClntPosnups& rhs) noexcept
    {
        std::swap(clnt_, rhs.clnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_clnt_first_posnup(clnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_clnt_first_posnup(clnt_);
    }
    Iterator
    end() noexcept
    {
        return DBR_CLNT_END_POSNUP;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_CLNT_END_POSNUP;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_clnt_last_posnup(clnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_clnt_last_posnup(clnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_CLNT_END_POSNUP;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_CLNT_END_POSNUP;
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
        return dbr_clnt_empty_posnup(clnt_);
    }
};

/** @} */

/**
 * @addtogroup ClntView
 * @{
 */

class ClntViews {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrView Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_clnt_view_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_clnt_view_entry(const_cast<Node*>(node));
        }
    };
    DbrClnt clnt_;
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
    ClntViews(DbrClnt clnt) noexcept
    : clnt_{clnt}
    {
    }
    void
    swap(ClntViews& rhs) noexcept
    {
        std::swap(clnt_, rhs.clnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_clnt_first_view(clnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_clnt_first_view(clnt_);
    }
    Iterator
    end() noexcept
    {
        return DBR_CLNT_END_VIEW;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_CLNT_END_VIEW;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_clnt_last_view(clnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_clnt_last_view(clnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_CLNT_END_VIEW;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_CLNT_END_VIEW;
    }

    // Find.

    Iterator
    find(DbrIden cid, DbrDate settl_date) noexcept
    {
        return dbr_clnt_find_view(clnt_, cid, settl_date);
    }
    ConstIterator
    find(DbrIden cid, DbrDate settl_date) const noexcept
    {
        return dbr_clnt_find_view(clnt_, cid, settl_date);
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
        return dbr_clnt_empty_view(clnt_);
    }
};

class ClntViewups {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrView Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_clnt_viewup_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_clnt_viewup_entry(const_cast<Node*>(node));
        }
    };
    DbrClnt clnt_;
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
    ClntViewups(DbrClnt clnt) noexcept
    : clnt_{clnt}
    {
    }
    void
    swap(ClntViewups& rhs) noexcept
    {
        std::swap(clnt_, rhs.clnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_clnt_first_viewup(clnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_clnt_first_viewup(clnt_);
    }
    Iterator
    end() noexcept
    {
        return DBR_CLNT_END_VIEWUP;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_CLNT_END_VIEWUP;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_clnt_last_viewup(clnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_clnt_last_viewup(clnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_CLNT_END_VIEWUP;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_CLNT_END_VIEWUP;
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
        return dbr_clnt_empty_viewup(clnt_);
    }
};

/** @} */

/**
 * @addtogroup Clnt
 * @{
 */

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
    Clnt(const char* sess, void* ctx, const char* mdaddr, const char* traddr,
         DbrIden seed, DbrPool pool)
        : impl_{dbr_clnt_create(sess, ctx, mdaddr, traddr, seed, pool)}
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
    /**
     * @addtogroup ClntRec
     * @{
     */
    template <int TypeN>
    ClntRecs<TypeN>
    recs() const noexcept
    {
        return ClntRecs<TypeN>{impl_};
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
    ClntContrRecs
    crecs() const noexcept
    {
        return ClntContrRecs{impl_};
    }
    /** @} */
    Trader
    trader(DbrRec& trec) const noexcept
    {
        return Trader{dbr_clnt_trader(impl_, &trec)};
    }
    Accnt
    accnt(DbrRec& arec) const
    {
        DbrAccnt accnt = dbr_clnt_accnt(impl_, &arec);
        if (!accnt)
            throw_exception();
        return Accnt{accnt};
    }
    DbrIden
    logon(DbrTrader trader, DbrMillis ms)
    {
        DbrIden req_id = dbr_clnt_logon(impl_, trader, ms);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    logoff(DbrTrader trader, DbrMillis ms)
    {
        DbrIden req_id = dbr_clnt_logoff(impl_, trader, ms);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    place(DbrTrader trader, DbrAccnt accnt, DbrRec& crec, DbrDate settl_date, const char* ref,
          int action, DbrTicks ticks, DbrLots lots, DbrLots min_lots, DbrMillis ms)
    {
        DbrIden req_id = dbr_clnt_place(impl_, trader, accnt, &crec, settl_date, ref, action,
                                        ticks, lots, min_lots, ms);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    revise(DbrTrader trader, DbrIden id, DbrLots lots, DbrMillis ms)
    {
        DbrIden req_id = dbr_clnt_revise_id(impl_, trader, id, lots, ms);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    revise(DbrTrader trader, const char* ref, DbrLots lots, DbrMillis ms)
    {
        DbrIden req_id = dbr_clnt_revise_ref(impl_, trader, ref, lots, ms);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    cancel(DbrTrader trader, DbrIden id, DbrMillis ms)
    {
        DbrIden req_id = dbr_clnt_cancel_id(impl_, trader, id, ms);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    cancel(DbrTrader trader, const char* ref, DbrMillis ms)
    {
        DbrIden req_id = dbr_clnt_cancel_ref(impl_, trader, ref, ms);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    ack_trade(DbrTrader trader, DbrIden id, DbrMillis ms)
    {
        DbrIden req_id = dbr_clnt_ack_trade(impl_, trader, id, ms);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    bool
    ready() const noexcept
    {
        return dbr_clnt_ready(impl_) == DBR_TRUE;
    }
    DbrIden
    settimer(DbrMillis absms)
    {
        const DbrIden id = dbr_clnt_settimer(impl_, absms);
        if (id < 0)
            throw_exception();
        return id;
    }
    void
    canceltimer(DbrIden id)
    {
        dbr_clnt_canceltimer(impl_, id);
    }
    zmq_pollitem_t*
    setitems(zmq_pollitem_t* items, int nitems)
    {
        items = dbr_clnt_setitems(impl_, items, nitems);
        if (!items)
            throw_exception();
        return items;
    }
    int
    poll(DbrMillis ms, DbrHandler handler)
    {
        const int nevents = dbr_clnt_poll(impl_, ms, handler);
        if (nevents < 0)
            throw_exception();
        return nevents;
    }
    void
    clear() noexcept
    {
        dbr_clnt_clear(impl_);
    }
    /**
     * @addtogroup ClntExec
     * @{
     */
    ClntExecs
    execs() const noexcept
    {
        return ClntExecs{impl_};
    }
    /** @} */
    /**
     * @addtogroup ClntPosn
     * @{
     */
    ClntPosnups
    posnups() const noexcept
    {
        return ClntPosnups{impl_};
    }
    /** @} */
    /**
     * @addtogroup ClntView
     * @{
     */
    ClntViews
    views() const noexcept
    {
        return ClntViews{impl_};
    }
    ClntViewups
    viewups() const noexcept
    {
        return ClntViewups{impl_};
    }
    /** @} */
};

/** @} */

} // dbr

#endif // DBRPP_CLNT_HPP
