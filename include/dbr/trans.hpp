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
#ifndef DBR_TRANS_HPP
#define DBR_TRANS_HPP

#include <dbr/iter.hpp>
#include <dbr/match.hpp>
#include <dbr/node.hpp>

#include <dbr/conv.h>
#include <dbr/ctx.h>

namespace dbr {

struct TransMatchPolicy : NodeTraits<DbrSlNode> {
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

class TransMatches {
    DbrTrans trans_;
public:
    typedef TransMatchPolicy::Entry ValueType;
    typedef TransMatchPolicy::Entry* Pointer;
    typedef TransMatchPolicy::Entry& Reference;
    typedef const TransMatchPolicy::Entry* ConstPointer;
    typedef const TransMatchPolicy::Entry& ConstReference;

    typedef ForwardIterator<TransMatchPolicy> Iterator;
    typedef ConstForwardIterator<TransMatchPolicy> ConstIterator;

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

    // Copy semantics.

    Trans(const Trans&) = delete;

    Trans&
    operator =(const Trans&) = delete;

    explicit
    operator DbrTrans*() noexcept
    {
        return &impl_;
    }
    void
    reset() noexcept
    {
        if (impl_.first_match) {
            dbr_ctx_free_matches(ctx_, impl_.first_match);
            impl_.first_match = nullptr;
        }
    }
    Order
    new_order() const noexcept
    {
        return *impl_.new_order;
    }
    Posn
    new_posn() const noexcept
    {
        return *impl_.new_posn;
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

#endif // DBR_TRANS_HPP
