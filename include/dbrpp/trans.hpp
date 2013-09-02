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
#ifndef DBRPP_TRANS_HPP
#define DBRPP_TRANS_HPP

#include <dbrpp/iter.hpp>
#include <dbrpp/slnode.hpp>
#include <dbrpp/match.hpp>

#include <dbr/ctx.h>

namespace dbr {

class TransMatches {
    struct Policy : NodeTraits<DbrSlNode> {
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
    DbrTrans trans_;
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
    TransMatches(DbrTrans trans) noexcept
    : trans_(trans)
    {
    }
    void
    swap(TransMatches& rhs) noexcept
    {
        std::swap(trans_, rhs.trans_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return trans_.first_match;
    }
    ConstIterator
    begin() const noexcept
    {
        return trans_.first_match;
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
        return trans_.taken;
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

class Trans {
    DbrCtx ctx_;
    DbrTrans impl_;
public:
    ~Trans() noexcept
    {
        reset();
    }
    explicit
    Trans(DbrCtx ctx) noexcept
    : ctx_(ctx)
    {
        impl_.first_match = nullptr;
    }
    operator DbrTrans&() noexcept
    {
        return impl_;
    }
    DbrTrans*
    c_arg() noexcept
    {
        return &impl_;
    }

    // Copy semantics.

    Trans(const Trans&) = delete;

    Trans&
    operator =(const Trans&) = delete;

    void
    reset() noexcept
    {
        if (impl_.first_match) {
            dbr_ctx_free_matches(ctx_, impl_.first_match);
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
    TransMatches
    matches() const noexcept
    {
        return TransMatches(impl_);
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

#endif // DBRPP_TRANS_HPP
