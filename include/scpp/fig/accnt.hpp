/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_FIG_ACCNT_HPP
#define SCPP_FIG_ACCNT_HPP

#include <scpp/fig/sess.hpp>

#include <scpp/elm/rec.hpp>

#include <scpp/ash/iter.hpp>
#include <scpp/ash/rbnode.hpp>

#include <sc/fig/accnt.h>

#include <iostream>
#include <limits>

namespace sc {

/**
 * @addtogroup Accnt
 * @{
 */

class AccntTraders {
    struct Policy : NodeTraits<ScRbNode> {
        typedef ScPerm Entry;
        static Entry*
        entry(Node* node)
        {
            return sc_accnt_trader_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return sc_accnt_trader_entry(const_cast<Node*>(node));
        }
    };
    ScAccnt accnt_;
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
    AccntTraders(ScAccnt accnt) noexcept
        : accnt_{accnt}
    {
    }
    void
    swap(AccntTraders& rhs) noexcept
    {
        std::swap(accnt_, rhs.accnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return sc_accnt_first_trader(accnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return sc_accnt_first_trader(accnt_);
    }
    Iterator
    end() noexcept
    {
        return SC_ACCNT_END_TRADER;
    }
    ConstIterator
    end() const noexcept
    {
        return SC_ACCNT_END_TRADER;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return sc_accnt_last_trader(accnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return sc_accnt_last_trader(accnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return SC_ACCNT_END_TRADER;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return SC_ACCNT_END_TRADER;
    }

    // Find.

    Iterator
    find(ScTicks ticks) noexcept
    {
        return sc_accnt_find_trader_id(accnt_, ticks);
    }
    ConstIterator
    find(ScTicks ticks) const noexcept
    {
        return sc_accnt_find_trader_id(accnt_, ticks);
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
        return sc_accnt_empty_trader(accnt_);
    }
};

class AccntGiveups {
    struct Policy : NodeTraits<ScRbNode> {
        typedef ScPerm Entry;
        static Entry*
        entry(Node* node)
        {
            return sc_accnt_giveup_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return sc_accnt_giveup_entry(const_cast<Node*>(node));
        }
    };
    ScAccnt accnt_;
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
    AccntGiveups(ScAccnt accnt) noexcept
        : accnt_{accnt}
    {
    }
    void
    swap(AccntGiveups& rhs) noexcept
    {
        std::swap(accnt_, rhs.accnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return sc_accnt_first_giveup(accnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return sc_accnt_first_giveup(accnt_);
    }
    Iterator
    end() noexcept
    {
        return SC_ACCNT_END_GIVEUP;
    }
    ConstIterator
    end() const noexcept
    {
        return SC_ACCNT_END_GIVEUP;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return sc_accnt_last_giveup(accnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return sc_accnt_last_giveup(accnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return SC_ACCNT_END_GIVEUP;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return SC_ACCNT_END_GIVEUP;
    }

    // Find.

    Iterator
    find(ScTicks ticks) noexcept
    {
        return sc_accnt_find_giveup_id(accnt_, ticks);
    }
    ConstIterator
    find(ScTicks ticks) const noexcept
    {
        return sc_accnt_find_giveup_id(accnt_, ticks);
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
        return sc_accnt_empty_giveup(accnt_);
    }
};

class AccntOrders {
    struct Policy : NodeTraits<ScRbNode> {
        typedef ScOrder Entry;
        static Entry*
        entry(Node* node)
        {
            return sc_accnt_order_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return sc_accnt_order_entry(const_cast<Node*>(node));
        }
    };
    ScAccnt accnt_;
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
    AccntOrders(ScAccnt accnt) noexcept
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
        return sc_accnt_first_order(accnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return sc_accnt_first_order(accnt_);
    }
    Iterator
    end() noexcept
    {
        return SC_ACCNT_END_ORDER;
    }
    ConstIterator
    end() const noexcept
    {
        return SC_ACCNT_END_ORDER;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return sc_accnt_last_order(accnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return sc_accnt_last_order(accnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return SC_ACCNT_END_ORDER;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return SC_ACCNT_END_ORDER;
    }

    // Find.

    Iterator
    find(ScTicks ticks) noexcept
    {
        return sc_accnt_find_order_id(accnt_, ticks);
    }
    ConstIterator
    find(ScTicks ticks) const noexcept
    {
        return sc_accnt_find_order_id(accnt_, ticks);
    }
    Iterator
    find(const char* ref) noexcept
    {
        ScOrder* const order = sc_accnt_find_order_ref(accnt_, ref);
        return order ? &order->accnt_node_ : end();
    }
    ConstIterator
    find(const char* ref) const noexcept
    {
        ScOrder* const order = sc_accnt_find_order_ref(accnt_, ref);
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
        return sc_accnt_empty_order(accnt_);
    }
};

class AccntTrades {
    struct Policy : NodeTraits<ScRbNode> {
        typedef ScExec Entry;
        static Entry*
        entry(Node* node)
        {
            return sc_accnt_trade_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return sc_accnt_trade_entry(const_cast<Node*>(node));
        }
    };
    ScAccnt accnt_;
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
    AccntTrades(ScAccnt accnt) noexcept
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
        return sc_accnt_first_trade(accnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return sc_accnt_first_trade(accnt_);
    }
    Iterator
    end() noexcept
    {
        return SC_ACCNT_END_TRADE;
    }
    ConstIterator
    end() const noexcept
    {
        return SC_ACCNT_END_TRADE;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return sc_accnt_last_trade(accnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return sc_accnt_last_trade(accnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return SC_ACCNT_END_TRADE;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return SC_ACCNT_END_TRADE;
    }

    // Find.

    Iterator
    find(ScTicks ticks) noexcept
    {
        return sc_accnt_find_trade_id(accnt_, ticks);
    }
    ConstIterator
    find(ScTicks ticks) const noexcept
    {
        return sc_accnt_find_trade_id(accnt_, ticks);
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
        return sc_accnt_empty_trade(accnt_);
    }
};

class AccntPosns {
    struct Policy : NodeTraits<ScRbNode> {
        typedef ScPosn Entry;
        static Entry*
        entry(Node* node)
        {
            return sc_accnt_posn_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return sc_accnt_posn_entry(const_cast<Node*>(node));
        }
    };
    ScAccnt accnt_;
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
    AccntPosns(ScAccnt accnt) noexcept
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
        return sc_accnt_first_posn(accnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return sc_accnt_first_posn(accnt_);
    }
    Iterator
    end() noexcept
    {
        return SC_ACCNT_END_POSN;
    }
    ConstIterator
    end() const noexcept
    {
        return SC_ACCNT_END_POSN;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return sc_accnt_last_posn(accnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return sc_accnt_last_posn(accnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return SC_ACCNT_END_POSN;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return SC_ACCNT_END_POSN;
    }

    // Find.

    Iterator
    find(ScIden id) noexcept
    {
        return sc_accnt_find_posn_id(accnt_, id);
    }
    ConstIterator
    find(ScIden id) const noexcept
    {
        return sc_accnt_find_posn_id(accnt_, id);
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
        return sc_accnt_empty_posn(accnt_);
    }
};

class Accnt {
    ScAccnt impl_;
public:
    explicit
    Accnt(ScAccnt impl) noexcept
        : impl_{impl}
    {
    }
    operator ScAccnt() const noexcept
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
        return AccntRecRef{*sc_accnt_rec(impl_)};
    }
    AccntTraders
    traders() const noexcept
    {
        return AccntTraders{impl_};
    }
    AccntGiveups
    giveups() const noexcept
    {
        return AccntGiveups{impl_};
    }
    AccntOrders
    orders() const noexcept
    {
        return AccntOrders{impl_};
    }
    AccntTrades
    trades() const noexcept
    {
        return AccntTrades{impl_};
    }
    AccntPosns
    posns() const noexcept
    {
        return AccntPosns{impl_};
    }
    ScBool
    logged_on() const noexcept
    {
        return sc_accnt_logged_on(impl_);
    }
    SessRef
    sess() const noexcept
    {
        return SessRef(*sc_accnt_sess(impl_));
    }
};

inline std::ostream&
operator <<(std::ostream& os, Accnt accnt)
{
    return os << "id=" << accnt.rec().id();
}

/** @} */

} // sc

#endif // SCPP_FIG_ACCNT_HPP
