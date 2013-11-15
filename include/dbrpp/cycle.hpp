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
#ifndef DBRPP_CYCLE_HPP
#define DBRPP_CYCLE_HPP

#include <dbrpp/exec.hpp>
#include <dbrpp/iter.hpp>
#include <dbrpp/order.hpp>
#include <dbrpp/slnode.hpp>

#include <limits>

namespace dbr {

class CyclePosns {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrPosn Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_cycle_posn_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_cycle_posn_entry(const_cast<Node*>(node));
        }
    };
    const DbrCycle* cycle_;
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
    CyclePosns(const DbrCycle& cycle) noexcept
        : cycle_{&cycle}
    {
    }
    void
    swap(CyclePosns& rhs) noexcept
    {
        std::swap(cycle_, rhs.cycle_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return cycle_->first_posn;
    }
    ConstIterator
    begin() const noexcept
    {
        return cycle_->first_posn;
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
        return cycle_->first_posn == nullptr;
    }
};

class CycleExecs {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrExec Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_cycle_exec_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_cycle_exec_entry(const_cast<Node*>(node));
        }
    };
    const DbrCycle* cycle_;
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
    CycleExecs(const DbrCycle& cycle) noexcept
        : cycle_{&cycle}
    {
    }
    void
    swap(CycleExecs& rhs) noexcept
    {
        std::swap(cycle_, rhs.cycle_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return cycle_->first_exec;
    }
    ConstIterator
    begin() const noexcept
    {
        return cycle_->first_exec;
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
        return cycle_->first_exec == nullptr;
    }
};

class Cycle {
    DbrCycle impl_;
public:
    operator DbrCycle&() noexcept
    {
        return impl_;
    }
    DbrCycle*
    c_arg() noexcept
    {
        return &impl_;
    }
    OrderRef
    new_order() const noexcept
    {
        return OrderRef{*impl_.new_order};
    }
    CyclePosns
    posns() const noexcept
    {
        return CyclePosns{impl_};
    }
    CycleExecs
    execs() const noexcept
    {
        return CycleExecs{impl_};
    }
};

} // dbr

#endif // DBRPP_CYCLE_HPP
