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

typedef ClntRecs<DBR_ENTITY_ACCNT> ClntAccntRecs;
typedef ClntRecs<DBR_ENTITY_CONTR> ClntContrRecs;

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
    find(DbrIden cid, DbrJd settl_day) noexcept
    {
        return dbr_clnt_find_view(clnt_, cid, settl_day);
    }
    ConstIterator
    find(DbrIden cid, DbrJd settl_day) const noexcept
    {
        return dbr_clnt_find_view(clnt_, cid, settl_day);
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
    Clnt(void* zctx, const DbrUuid uuid, const char* mdaddr, const char* traddr,
         DbrIden seed, DbrMillis tmout, DbrPool pool)
        : impl_{dbr_clnt_create(zctx, uuid, mdaddr, traddr, seed, tmout, pool)}
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
    void
    reset() noexcept
    {
        dbr_clnt_reset(impl_);
    }
    DbrIden
    close()
    {
        DbrIden req_id = dbr_clnt_close(impl_);
        if (req_id < 0)
            throw_exception();
        return req_id;
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
    Accnt
    accnt(DbrRec& arec) const
    {
        DbrAccnt accnt = dbr_clnt_accnt(impl_, &arec);
        if (!accnt)
            throw_exception();
        return Accnt{accnt};
    }
    DbrIden
    logon(DbrAccnt user)
    {
        DbrIden req_id = dbr_clnt_logon(impl_, user);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    logoff(DbrAccnt user)
    {
        DbrIden req_id = dbr_clnt_logoff(impl_, user);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    place(DbrAccnt user, DbrAccnt group, DbrRec& crec, DbrJd settl_day, const char* ref,
          int action, DbrTicks ticks, DbrLots lots, DbrLots min_lots)
    {
        DbrIden req_id = dbr_clnt_place(impl_, user, group, &crec, settl_day, ref, action,
                                        ticks, lots, min_lots);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    revise(DbrAccnt user, DbrIden id, DbrLots lots)
    {
        DbrIden req_id = dbr_clnt_revise_id(impl_, user, id, lots);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    revise(DbrAccnt user, const char* ref, DbrLots lots)
    {
        DbrIden req_id = dbr_clnt_revise_ref(impl_, user, ref, lots);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    cancel(DbrAccnt user, DbrIden id)
    {
        DbrIden req_id = dbr_clnt_cancel_id(impl_, user, id);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    cancel(DbrAccnt user, const char* ref)
    {
        DbrIden req_id = dbr_clnt_cancel_ref(impl_, user, ref);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    DbrIden
    ack_trade(DbrAccnt user, DbrIden id)
    {
        DbrIden req_id = dbr_clnt_ack_trade(impl_, user, id);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    bool
    is_ready() const noexcept
    {
        return dbr_clnt_is_ready(impl_) == DBR_TRUE;
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
    bool
    dispatch(DbrMillis ms, DbrHandler handler)
    {
        const int ret = dbr_clnt_dispatch(impl_, ms, handler);
        if (ret < 0)
            throw_exception();
        return ret == DBR_TRUE;
    }
    /**
     * @addtogroup ClntView
     * @{
     */
    ClntViews
    views() const noexcept
    {
        return ClntViews{impl_};
    }
    /** @} */
    const unsigned char*
    uuid() const noexcept
    {
        return dbr_clnt_uuid(impl_);
    }
};

/** @} */

} // dbr

#endif // DBRPP_CLNT_HPP
