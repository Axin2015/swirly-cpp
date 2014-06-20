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
#ifndef DBRPP_ACCNT_HPP
#define DBRPP_ACCNT_HPP

#include <dbrpp/iter.hpp>
#include <dbrpp/rbnode.hpp>
#include <dbrpp/rec.hpp>
#include <dbrpp/sess.hpp>

#include <dbr/fig/accnt.h>

#include <iostream>
#include <limits>

namespace dbr {

/**
 * @addtogroup AccntUser
 * @{
 */

class AccntUsers {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrMemb Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_accnt_user_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_accnt_user_entry(const_cast<Node*>(node));
        }
    };
    DbrAccnt accnt_;
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
    AccntUsers(DbrAccnt accnt) noexcept
        : accnt_{accnt}
    {
    }
    void
    swap(AccntUsers& rhs) noexcept
    {
        std::swap(accnt_, rhs.accnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_accnt_first_user(accnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_accnt_first_user(accnt_);
    }
    Iterator
    end() noexcept
    {
        return DBR_ACCNT_END_USER;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_ACCNT_END_USER;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_accnt_last_user(accnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_accnt_last_user(accnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_ACCNT_END_USER;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_ACCNT_END_USER;
    }

    // Find.

    Iterator
    find(DbrTicks ticks) noexcept
    {
        return dbr_accnt_find_user_id(accnt_, ticks);
    }
    ConstIterator
    find(DbrTicks ticks) const noexcept
    {
        return dbr_accnt_find_user_id(accnt_, ticks);
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
        return dbr_accnt_empty_user(accnt_);
    }
};

/** @} */

/**
 * @addtogroup AccntGroup
 * @{
 */

class AccntGroups {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrMemb Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_accnt_group_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_accnt_group_entry(const_cast<Node*>(node));
        }
    };
    DbrAccnt accnt_;
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
    AccntGroups(DbrAccnt accnt) noexcept
        : accnt_{accnt}
    {
    }
    void
    swap(AccntGroups& rhs) noexcept
    {
        std::swap(accnt_, rhs.accnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_accnt_first_group(accnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_accnt_first_group(accnt_);
    }
    Iterator
    end() noexcept
    {
        return DBR_ACCNT_END_GROUP;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_ACCNT_END_GROUP;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_accnt_last_group(accnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_accnt_last_group(accnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_ACCNT_END_GROUP;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_ACCNT_END_GROUP;
    }

    // Find.

    Iterator
    find(DbrTicks ticks) noexcept
    {
        return dbr_accnt_find_group_id(accnt_, ticks);
    }
    ConstIterator
    find(DbrTicks ticks) const noexcept
    {
        return dbr_accnt_find_group_id(accnt_, ticks);
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
        return dbr_accnt_empty_group(accnt_);
    }
};

/** @} */

/**
 * @addtogroup AccntOrder
 * @{
 */

class AccntOrders {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrOrder Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_accnt_order_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_accnt_order_entry(const_cast<Node*>(node));
        }
    };
    DbrAccnt accnt_;
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
    AccntOrders(DbrAccnt accnt) noexcept
        : accnt_{accnt}
    {
    }
    void
    swap(AccntOrders& rhs) noexcept
    {
        std::swap(accnt_, rhs.accnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_accnt_first_order(accnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_accnt_first_order(accnt_);
    }
    Iterator
    end() noexcept
    {
        return DBR_ACCNT_END_ORDER;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_ACCNT_END_ORDER;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_accnt_last_order(accnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_accnt_last_order(accnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_ACCNT_END_ORDER;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_ACCNT_END_ORDER;
    }

    // Find.

    Iterator
    find(DbrTicks ticks) noexcept
    {
        return dbr_accnt_find_order_id(accnt_, ticks);
    }
    ConstIterator
    find(DbrTicks ticks) const noexcept
    {
        return dbr_accnt_find_order_id(accnt_, ticks);
    }
    Iterator
    find(const char* ref) noexcept
    {
        DbrOrder* const order = dbr_accnt_find_order_ref(accnt_, ref);
        return order ? &order->accnt_node_ : end();
    }
    ConstIterator
    find(const char* ref) const noexcept
    {
        DbrOrder* const order = dbr_accnt_find_order_ref(accnt_, ref);
        return order ? &order->accnt_node_ : end();
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
        return dbr_accnt_empty_order(accnt_);
    }
};

/** @} */

/**
 * @addtogroup AccntTrade
 * @{
 */

class AccntTrades {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrExec Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_accnt_trade_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_accnt_trade_entry(const_cast<Node*>(node));
        }
    };
    DbrAccnt accnt_;
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
    AccntTrades(DbrAccnt accnt) noexcept
        : accnt_{accnt}
    {
    }
    void
    swap(AccntTrades& rhs) noexcept
    {
        std::swap(accnt_, rhs.accnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_accnt_first_trade(accnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_accnt_first_trade(accnt_);
    }
    Iterator
    end() noexcept
    {
        return DBR_ACCNT_END_TRADE;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_ACCNT_END_TRADE;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_accnt_last_trade(accnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_accnt_last_trade(accnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_ACCNT_END_TRADE;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_ACCNT_END_TRADE;
    }

    // Find.

    Iterator
    find(DbrTicks ticks) noexcept
    {
        return dbr_accnt_find_trade_id(accnt_, ticks);
    }
    ConstIterator
    find(DbrTicks ticks) const noexcept
    {
        return dbr_accnt_find_trade_id(accnt_, ticks);
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
        return dbr_accnt_empty_trade(accnt_);
    }
};

/** @} */

/**
 * @addtogroup AccntPosn
 * @{
 */

class AccntPosns {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrPosn Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_accnt_posn_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_accnt_posn_entry(const_cast<Node*>(node));
        }
    };
    DbrAccnt accnt_;
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
    AccntPosns(DbrAccnt accnt) noexcept
    : accnt_{accnt}
    {
    }
    void
    swap(AccntPosns& rhs) noexcept
    {
        std::swap(accnt_, rhs.accnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_accnt_first_posn(accnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_accnt_first_posn(accnt_);
    }
    Iterator
    end() noexcept
    {
        return DBR_ACCNT_END_POSN;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_ACCNT_END_POSN;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_accnt_last_posn(accnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_accnt_last_posn(accnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_ACCNT_END_POSN;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_ACCNT_END_POSN;
    }

    // Find.

    Iterator
    find(DbrIden id) noexcept
    {
        return dbr_accnt_find_posn_id(accnt_, id);
    }
    ConstIterator
    find(DbrIden id) const noexcept
    {
        return dbr_accnt_find_posn_id(accnt_, id);
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
        return dbr_accnt_empty_posn(accnt_);
    }
};

/** @} */

/**
 * @addtogroup Accnt
 * @{
 */

class Accnt {
    DbrAccnt impl_;
public:
    explicit
    Accnt(DbrAccnt impl) noexcept
        : impl_{impl}
    {
    }
    operator DbrAccnt() const noexcept
    {
        return impl_;
    }
    bool
    operator ==(Accnt rhs) const noexcept
    {
        return rec().id() == rhs.rec().id();
    }
    bool
    operator !=(Accnt rhs) const noexcept
    {
        return rec().id() != rhs.rec().id();
    }
    AccntRecRef
    rec() const noexcept
    {
        return AccntRecRef{*dbr_accnt_rec(impl_)};
    }
    /**
     * @addtogroup AccntUser
     * @{
     */
    AccntUsers
    users() const noexcept
    {
        return AccntUsers{impl_};
    }
    /** @} */
    /**
     * @addtogroup AccntGroup
     * @{
     */
    AccntGroups
    groups() const noexcept
    {
        return AccntGroups{impl_};
    }
    /** @} */
    /**
     * @addtogroup AccntOrder
     * @{
     */
    AccntOrders
    orders() const noexcept
    {
        return AccntOrders{impl_};
    }
    /** @} */
    /**
     * @addtogroup AccntTrade
     * @{
     */
    AccntTrades
    trades() const noexcept
    {
        return AccntTrades{impl_};
    }
    /** @} */
    /**
     * @addtogroup AccntPosn
     * @{
     */
    AccntPosns
    posns() const noexcept
    {
        return AccntPosns{impl_};
    }
    /** @} */
    DbrBool
    logged_on() const noexcept
    {
        return dbr_accnt_logged_on(impl_);
    }
    SessRef
    sess() const noexcept
    {
        return SessRef(*dbr_accnt_sess(impl_));
    }
};

inline std::ostream&
operator <<(std::ostream& os, Accnt accnt)
{
    return os << "id=" << accnt.rec().id();
}

/** @} */

} // dbr

#endif // DBRPP_ACCNT_HPP
