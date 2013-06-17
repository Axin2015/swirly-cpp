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
#ifndef DBR_REC_HPP
#define DBR_REC_HPP

#include <dbr/iter.hpp>
#include <dbr/node.hpp>

#include <dbr/conv.h>
#include <dbr/ctx.h>

#include <limits>

namespace dbr {

struct RecPolicy : NodeTraits<DbrSlNode> {
    typedef DbrRec Entry;
    static Entry*
    entry(Node* node)
    {
        return dbr_rec_entry(node);
    }
    static const Entry*
    entry(const Node* node)
    {
        return dbr_rec_entry(const_cast<Node*>(node));
    }
};

template <int TypeN>
class Recs {
    DbrCtx ctx_;
public:
    typedef RecPolicy::Entry ValueType;
    typedef RecPolicy::Entry* Pointer;
    typedef RecPolicy::Entry& Reference;
    typedef const RecPolicy::Entry* ConstPointer;
    typedef const RecPolicy::Entry& ConstReference;

    typedef ForwardIterator<RecPolicy> Iterator;
    typedef ConstForwardIterator<RecPolicy> ConstIterator;

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
    Recs(DbrCtx ctx) noexcept
    : ctx_(ctx)
    {
    }
    void
    swap(Recs& rhs) noexcept
    {
        std::swap(ctx_, rhs.ctx_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return Iterator(dbr_ctx_first_rec(ctx_, TypeN, nullptr));
    }
    ConstIterator
    begin() const noexcept
    {
        return ConstIterator(dbr_ctx_first_rec(ctx_, TypeN, nullptr));
    }
    Iterator
    end() noexcept
    {
        return Iterator(dbr_ctx_end_rec(ctx_));
    }
    ConstIterator
    end() const noexcept
    {
        return ConstIterator(dbr_ctx_end_rec(ctx_));
    }
    Iterator
    find(DbrIden id) noexcept
    {
        return Iterator(dbr_ctx_find_rec_id(ctx_, TypeN, id));
    }
    ConstIterator
    find(DbrIden id) const noexcept
    {
        return ConstIterator(dbr_ctx_find_rec_id(ctx_, TypeN, id));
    }
    Iterator
    find(const char* mnem) noexcept
    {
        return Iterator(dbr_ctx_find_rec_mnem(ctx_, TypeN, mnem));
    }
    ConstIterator
    find(const char* mnem) const noexcept
    {
        return ConstIterator(dbr_ctx_find_rec_mnem(ctx_, TypeN, mnem));
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
        size_t size;
        dbr_ctx_first_rec(ctx_, TypeN, &size);
        return size;
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

typedef Recs<DBR_INSTR> InstrRecs;
typedef Recs<DBR_MARKET> MarketRecs;
typedef Recs<DBR_TRADER> TraderRecs;
typedef Recs<DBR_ACCNT> AccntRecs;

} // dbr

#endif // DBR_REC_HPP
