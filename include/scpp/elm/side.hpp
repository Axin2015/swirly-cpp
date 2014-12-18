/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_SIDE_HPP
#define SCPP_ELM_SIDE_HPP

#include <scpp/ash/dlnode.hpp>
#include <scpp/ash/except.hpp>
#include <scpp/ash/iter.hpp>
#include <scpp/ash/rbnode.hpp>

#include <sc/elm/side.h>

#include <iostream>
#include <limits>

namespace sc {

/**
 * @addtogroup Side
 * @{
 */

class SideOrders {
    struct Policy : NodeTraits<ScDlNode> {
        typedef ScOrder Entry;
        static Entry*
        entry(Node* node)
        {
            return sc_side_order_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return sc_side_order_entry(const_cast<Node*>(node));
        }
    };
    ScSide* side_;
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
    SideOrders(ScSide& side) noexcept
        : side_{&side}
    {
    }
    void
    swap(SideOrders& rhs) noexcept
    {
        std::swap(side_, rhs.side_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return sc_side_first_order(side_);
    }
    ConstIterator
    begin() const noexcept
    {
        return sc_side_first_order(side_);
    }
    Iterator
    end() noexcept
    {
        return sc_side_end_order(side_);
    }
    ConstIterator
    end() const noexcept
    {
        return sc_side_end_order(side_);
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return sc_side_last_order(side_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return sc_side_last_order(side_);
    }
    ReverseIterator
    rend() noexcept
    {
        return sc_side_end_order(side_);
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return sc_side_end_order(side_);
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
        return begin() == end();
    }
};

class SideLevels {
    struct Policy : NodeTraits<ScRbNode> {
        typedef ScLevel Entry;
        static Entry*
        entry(Node* node)
        {
            return sc_side_level_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return sc_side_level_entry(const_cast<Node*>(node));
        }
    };
    ScSide* side_;
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
    SideLevels(ScSide& side) noexcept
        : side_{&side}
    {
    }
    void
    swap(SideLevels& rhs) noexcept
    {
        std::swap(side_, rhs.side_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return sc_side_first_level(side_);
    }
    ConstIterator
    begin() const noexcept
    {
        return sc_side_first_level(side_);
    }
    Iterator
    end() noexcept
    {
        return SC_SIDE_END_LEVEL;
    }
    ConstIterator
    end() const noexcept
    {
        return SC_SIDE_END_LEVEL;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return sc_side_last_level(side_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return sc_side_last_level(side_);
    }
    ReverseIterator
    rend() noexcept
    {
        return SC_SIDE_END_LEVEL;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return SC_SIDE_END_LEVEL;
    }

    // Find.

    Iterator
    find(ScTicks ticks) noexcept
    {
        return sc_side_find_level(side_, ticks);
    }
    ConstIterator
    find(ScTicks ticks) const noexcept
    {
        return sc_side_find_level(side_, ticks);
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
        return sc_side_empty_level(side_);
    }
};

class Side {
    mutable ScSide impl_;
public:
    ~Side() noexcept
    {
        sc_side_term(&impl_);
    }
    explicit
    Side(ScPool pool) noexcept
    {
        sc_side_init(&impl_, pool);
    }
    operator ScSide&() noexcept
    {
        return impl_;
    }
    ScSide*
    c_arg() noexcept
    {
        return &impl_;
    }

    // Copy semantics.

    Side(const Side&) = delete;

    Side&
    operator =(const Side&) = delete;

    void
    insert_order(ScOrder& order)
    {
        if (!sc_side_insert_order(&impl_, &order))
            throw_exception();
    }
    void
    remove_order(ScOrder& order) noexcept
    {
        sc_side_remove_order(&impl_, &order);
    }
    void
    take_order(ScOrder& order, ScLots lots, ScMillis now) noexcept
    {
        sc_side_take_order(&impl_, &order, lots, now);
    }
    void
    place_order(ScOrder& order, ScMillis now)
    {
        if (!sc_side_place_order(&impl_, &order, now))
            throw_exception();
    }
    void
    revise_order(ScOrder& order, ScLots lots, ScMillis now) noexcept
    {
        sc_side_revise_order(&impl_, &order, lots, now);
    }
    void
    cancel_order(ScOrder& order, ScMillis now) noexcept
    {
        sc_side_cancel_order(&impl_, &order, now);
    }
    SideOrders
    orders() const noexcept
    {
        return SideOrders{impl_};
    }
    SideLevels
    levels() const noexcept
    {
        return SideLevels{impl_};
    }
    ScTicks
    last_ticks() const noexcept
    {
        return sc_side_last_ticks(&impl_);
    }
    ScLots
    last_lots() const noexcept
    {
        return sc_side_last_lots(&impl_);
    }
    ScMillis
    last_time() const noexcept
    {
        return sc_side_last_time(&impl_);
    }
};

inline std::ostream&
operator <<(std::ostream& os, const Side& side)
{
    return os << "last_ticks=" << side.last_ticks()
              << ",last_lots=" << side.last_lots()
              << ",last_time=" << side.last_time();
}

class SideRef {
    ScSide* impl_;
public:
    explicit
    SideRef(ScSide& impl) noexcept
        : impl_{&impl}
    {
    }
    operator ScSide&() const noexcept
    {
        return *impl_;
    }
    ScSide*
    c_arg() const noexcept
    {
        return impl_;
    }
    SideOrders
    orders() const noexcept
    {
        return SideOrders{*impl_};
    }
    SideLevels
    levels() const noexcept
    {
        return SideLevels{*impl_};
    }
    ScTicks
    last_ticks() const noexcept
    {
        return sc_side_last_ticks(impl_);
    }
    ScLots
    last_lots() const noexcept
    {
        return sc_side_last_lots(impl_);
    }
    ScMillis
    last_time() const noexcept
    {
        return sc_side_last_time(impl_);
    }
};

inline std::ostream&
operator <<(std::ostream& os, SideRef side)
{
    return os << "last_ticks=" << side.last_ticks()
              << ",last_lots=" << side.last_lots()
              << ",last_time=" << side.last_time();
}

/** @} */

} // sc

#endif // SCPP_ELM_SIDE_HPP
