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
#ifndef DBR_SIDE_HPP
#define DBR_SIDE_HPP

#include <dbr/iter.hpp>
#include <dbr/node.hpp>

#include <dbr/conv.h>
#include <dbr/side.h>

#include <iostream>
#include <limits>

namespace dbr {

struct SideOrderPolicy : NodeTraits<DbrDlNode> {
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

class SideOrders {
    DbrSide side_;
public:
    typedef SideOrderPolicy::Entry ValueType;
    typedef SideOrderPolicy::Entry* Pointer;
    typedef SideOrderPolicy::Entry& Reference;
    typedef const SideOrderPolicy::Entry* ConstPointer;
    typedef const SideOrderPolicy::Entry& ConstReference;

    typedef BiDirectionalIterator<SideOrderPolicy> Iterator;
    typedef ConstBiDirectionalIterator<SideOrderPolicy> ConstIterator;
    typedef ReverseBiDirectionalIterator<SideOrderPolicy> ReverseIterator;
    typedef ConstReverseBiDirectionalIterator<SideOrderPolicy> ConstReverseIterator;

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
    SideOrders(DbrSide side) noexcept
    : side_(side)
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

class Side {
    DbrSide impl_;
public:
    Side(DbrSide impl) noexcept
        : impl_(impl)
    {
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
operator <<(std::ostream& os, Side side)
{
    return os << "last_ticks=" << side.last_ticks()
              << ",last_lots=" << side.last_lots()
              << ",last_time=" << side.last_time();
}

} // dbr

#endif // DBR_SIDE_HPP
