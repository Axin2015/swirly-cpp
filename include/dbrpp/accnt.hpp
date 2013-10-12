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
#ifndef DBRPP_ACCNT_HPP
#define DBRPP_ACCNT_HPP

#include <dbrpp/iter.hpp>
#include <dbrpp/rbnode.hpp>
#include <dbrpp/rec.hpp>

#include <dbr/accnt.h>

#include <iostream>
#include <limits>

namespace dbr {

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
    : accnt_(accnt)
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

class Accnt {
    DbrAccnt impl_;
public:
    explicit
    Accnt(DbrAccnt impl) noexcept
        : impl_(impl)
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
        return AccntRecRef(*dbr_accnt_rec(impl_));
    }
    AccntPosns
    posns() const noexcept
    {
        return AccntPosns(impl_);
    }
};

inline std::ostream&
operator <<(std::ostream& os, Accnt accnt)
{
    return os << "id=" << accnt.rec().id();
}

} // dbr

#endif // DBRPP_ACCNT_HPP
