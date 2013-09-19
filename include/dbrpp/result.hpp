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
#ifndef DBRPP_RESULT_HPP
#define DBRPP_RESULT_HPP

#include <dbrpp/iter.hpp>
#include <dbrpp/slnode.hpp>
#include <dbrpp/match.hpp>

#include <dbr/exch.h>

namespace dbr {

class ResultMatches {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrMatch Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_result_match_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_result_match_entry(const_cast<Node*>(node));
        }
    };
    DbrResult result_;
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
    ResultMatches(DbrResult result) noexcept
    : result_(result)
    {
    }
    void
    swap(ResultMatches& rhs) noexcept
    {
        std::swap(result_, rhs.result_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return result_.first_match;
    }
    ConstIterator
    begin() const noexcept
    {
        return result_.first_match;
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
        return result_.taken;
    }
    SizeType
    max_size() const noexcept
    {
        return std::numeric_limits<SizeType>::max();
    }
    bool
    empty() const noexcept
    {
        return size() == 0;
    }
};

class Result {
    DbrExch exch_;
    DbrResult impl_;
public:
    ~Result() noexcept
    {
        reset();
    }
    explicit
    Result(DbrExch exch) noexcept
    : exch_(exch)
    {
        impl_.first_match = nullptr;
    }
    operator DbrResult&() noexcept
    {
        return impl_;
    }
    DbrResult*
    c_arg() noexcept
    {
        return &impl_;
    }

    // Copy semantics.

    Result(const Result&) = delete;

    Result&
    operator =(const Result&) = delete;

    void
    reset() noexcept
    {
        if (impl_.first_match) {
            dbr_exch_free_matches(exch_, impl_.first_match);
            impl_.first_match = nullptr;
        }
    }
    OrderRef
    new_order() const noexcept
    {
        return OrderRef(*impl_.new_order);
    }
    PosnRef
    new_posn() const noexcept
    {
        return PosnRef(*impl_.new_posn);
    }
    ResultMatches
    matches() const noexcept
    {
        return ResultMatches(impl_);
    }
    size_t
    count() const noexcept
    {
        return impl_.count;
    }
    DbrLots
    taken() const noexcept
    {
        return impl_.taken;
    }
};

} // dbr

#endif // DBRPP_RESULT_HPP
