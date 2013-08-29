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
#ifndef DBRPP_SIDE_HPP
#define DBRPP_SIDE_HPP

#include <dbrpp/dlnode.hpp>
#include <dbrpp/iter.hpp>
#include <dbrpp/rbnode.hpp>

#include <dbr/conv.h>
#include <dbr/side.h>

#include <iostream>
#include <limits>

namespace dbr {

class SideOrders {
    struct Policy : NodeTraits<DbrDlNode> {
        typedef DbrOrder Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_side_order_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_side_order_entry(const_cast<Node*>(node));
        }
    };
    DbrSide* side_;
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
    SideOrders(DbrSide& side) noexcept
    : side_(&side)
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
        return dbr_side_first_order(side_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_side_first_order(side_);
    }
    Iterator
    end() noexcept
    {
        return dbr_side_end_order(side_);
    }
    ConstIterator
    end() const noexcept
    {
        return dbr_side_end_order(side_);
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_side_last_order(side_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_side_last_order(side_);
    }
    ReverseIterator
    rend() noexcept
    {
        return dbr_side_end_order(side_);
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return dbr_side_end_order(side_);
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
    struct Policy : NodeTraits<DbrRbNode> {
        typedef DbrLevel Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_side_level_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_side_level_entry(const_cast<Node*>(node));
        }
    };
    DbrSide* side_;
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
    SideLevels(DbrSide& side) noexcept
    : side_(&side)
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
        return dbr_side_first_level(side_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_side_first_level(side_);
    }
    Iterator
    end() noexcept
    {
        return dbr_side_end_level(side_);
    }
    ConstIterator
    end() const noexcept
    {
        return dbr_side_end_level(side_);
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_side_last_level(side_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_side_last_level(side_);
    }
    ReverseIterator
    rend() noexcept
    {
        return dbr_side_end_level(side_);
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return dbr_side_end_level(side_);
    }

    // Find.

    Iterator
    find(DbrTicks ticks) noexcept
    {
        return dbr_side_find_level(side_, ticks);
    }
    ConstIterator
    find(DbrTicks ticks) const noexcept
    {
        return dbr_side_find_level(side_, ticks);
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
        return dbr_side_empty_level(side_);
    }
};

class SideRef {
    DbrSide* impl_;
public:
    explicit
    SideRef(DbrSide& impl) noexcept
        : impl_(&impl)
    {
    }
    operator DbrSide&() const noexcept
    {
        return *impl_;
    }
    DbrSide*
    c_arg() const noexcept
    {
        return impl_;
    }
    SideOrders
    orders() const noexcept
    {
        return SideOrders(*impl_);
    }
    SideLevels
    levels() const noexcept
    {
        return SideLevels(*impl_);
    }
    DbrTicks
    last_ticks() const noexcept
    {
        return dbr_side_last_ticks(impl_);
    }
    DbrLots
    last_lots() const noexcept
    {
        return dbr_side_last_lots(impl_);
    }
    DbrMillis
    last_time() const noexcept
    {
        return dbr_side_last_time(impl_);
    }
};

inline std::ostream&
operator <<(std::ostream& os, SideRef side)
{
    return os << "last_ticks=" << side.last_ticks()
              << ",last_lots=" << side.last_lots()
              << ",last_time=" << side.last_time();
}

} // dbr

#endif // DBRPP_SIDE_HPP
