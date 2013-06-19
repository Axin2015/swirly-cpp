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
#ifndef DBR_MATCH_HPP
#define DBR_MATCH_HPP

#include <dbr/order.hpp>
#include <dbr/posn.hpp>
#include <dbr/trade.hpp>

namespace dbr {

struct MatchPolicy : NodeTraits<DbrSlNode> {
    typedef DbrMatch Entry;
    static Entry*
    entry(Node* node)
    {
        return dbr_trans_match_entry(node);
    }
    static const Entry*
    entry(const Node* node)
    {
        return dbr_trans_match_entry(const_cast<Node*>(node));
    }
};

class Matches {
    DbrTrans trans_;
public:
    typedef MatchPolicy::Entry ValueType;
    typedef MatchPolicy::Entry* Pointer;
    typedef MatchPolicy::Entry& Reference;
    typedef const MatchPolicy::Entry* ConstPointer;
    typedef const MatchPolicy::Entry& ConstReference;

    typedef ForwardIterator<MatchPolicy> Iterator;
    typedef ConstForwardIterator<MatchPolicy> ConstIterator;

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
    Matches(DbrTrans trans) noexcept
    : trans_(trans)
    {
    }
    void
    swap(Matches& rhs) noexcept
    {
        std::swap(trans_, rhs.trans_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return Iterator(trans_.first_match);
    }
    ConstIterator
    begin() const noexcept
    {
        return ConstIterator(trans_.first_match);
    }
    Iterator
    end() noexcept
    {
        return Iterator();
    }
    ConstIterator
    end() const noexcept
    {
        return ConstIterator();
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
        return begin() == end();
    }
};

class Match {
    const DbrMatch& impl_;
public:
    explicit
    Match(const DbrMatch& impl) noexcept
        : impl_(impl)
    {
    }
    bool
    operator ==(Match rhs) const noexcept
    {
        return impl_.id == rhs.impl_.id;
    }
    bool
    operator !=(Match rhs) const noexcept
    {
        return impl_.id != rhs.impl_.id;
    }
    DbrIden
    id() const noexcept
    {
        return impl_.id;
    }
    Order
    maker_order() const noexcept
    {
        return Order(*impl_.maker_order);
    }
    Posn
    maker_posn() const noexcept
    {
        return Posn(*impl_.maker_posn);
    }
    DbrTicks
    ticks() const noexcept
    {
        return impl_.ticks;
    }
    DbrLots
    lots() const noexcept
    {
        return impl_.lots;
    }
    Trade
    taker_trade() const noexcept
    {
        return Trade(*impl_.taker_trade);
    }
    Trade
    maker_trade() const noexcept
    {
        return Trade(*impl_.maker_trade);
    }
};

inline std::ostream&
operator <<(std::ostream& os, Match match)
{
    return os << "id=" << match.id()
              << ",maker_order=" << match.maker_order().id()
              << ",maker_posn=" << match.maker_posn().id()
              << ",ticks=" << match.ticks()
              << ",lots=" << match.lots()
              << ",taker_trade=" << match.taker_trade().id()
              << ",maker_trade=" << match.maker_trade().id();
}
} // dbr

#endif // DBR_MATCH_HPP
