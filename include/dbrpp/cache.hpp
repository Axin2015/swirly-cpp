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
#ifndef DBRPP_CACHE_HPP
#define DBRPP_CACHE_HPP

#include <dbrpp/except.hpp>
#include <dbrpp/iter.hpp>
#include <dbrpp/slnode.hpp>

#include <dbr/cache.h>
#include <dbr/types.h>

#include <limits>

namespace dbr {

template <int TypeN>
class CacheRecs {
    struct Policy : NodeTraits<DbrSlNode> {
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
    DbrCache cache_;
public:
    typedef typename Policy::Entry ValueType;
    typedef typename Policy::Entry* Pointer;
    typedef typename Policy::Entry& Reference;
    typedef const typename Policy::Entry* ConstPointer;
    typedef const typename Policy::Entry& ConstReference;

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
    CacheRecs(DbrCache cache) noexcept
    : cache_(cache)
    {
    }
    void
    swap(CacheRecs& rhs) noexcept
    {
        std::swap(cache_, rhs.cache_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_cache_first_rec(cache_, TypeN, nullptr);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_cache_first_rec(cache_, TypeN, nullptr);
    }
    Iterator
    end() noexcept
    {
        return DBR_CACHE_END_REC;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_CACHE_END_REC;
    }
    Iterator
    find(DbrIden id) noexcept
    {
        return dbr_cache_find_rec_id(cache_, TypeN, id);
    }
    ConstIterator
    find(DbrIden id) const noexcept
    {
        return dbr_cache_find_rec_id(cache_, TypeN, id);
    }
    Iterator
    find(const char* mnem) noexcept
    {
        return dbr_cache_find_rec_mnem(cache_, TypeN, mnem);
    }
    ConstIterator
    find(const char* mnem) const noexcept
    {
        return dbr_cache_find_rec_mnem(cache_, TypeN, mnem);
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
        dbr_cache_first_rec(cache_, TypeN, &size);
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

typedef CacheRecs<DBR_TRADER> CacheTraderRecs;
typedef CacheRecs<DBR_ACCNT> CacheAccntRecs;
typedef CacheRecs<DBR_CONTR> CacheContrRecs;

class Cache {
    DbrCache impl_;
public:
    ~Cache() noexcept
    {
        if (impl_)
            dbr_cache_destroy(impl_);
    }
    constexpr
    Cache(decltype(nullptr)) noexcept
    : impl_(nullptr)
    {
    }
    explicit
    Cache(DbrPool pool)
        : impl_(dbr_cache_create(nullptr, pool))
    {
        if (!impl_)
            throw_exception();
    }
    Cache(void (*term_state)(struct DbrRec*), DbrPool pool)
        : impl_(dbr_cache_create(term_state, pool))
    {
        if (!impl_)
            throw_exception();
    }
    operator DbrCache() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Cache(const Cache&) = delete;

    Cache&
    operator =(const Cache&) = delete;

    // Move semantics.

    Cache(Cache&& rhs) noexcept
    : impl_(nullptr)
    {
        swap(rhs);
    }
    Cache&
    operator =(Cache&& rhs) noexcept
    {
        if (impl_) {
            dbr_cache_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(Cache& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    void
    emplace_recs(int type, DbrSlNode* first, size_t size)
    {
        dbr_cache_emplace_recs(impl_, type, first, size);
    }
    template <int TypeN>
    CacheRecs<TypeN>
    recs() const noexcept
    {
        return CacheRecs<TypeN>(impl_);
    }
    CacheContrRecs
    crecs() const noexcept
    {
        return CacheContrRecs(impl_);
    }
    CacheTraderRecs
    trecs() const noexcept
    {
        return CacheTraderRecs(impl_);
    }
    CacheAccntRecs
    arecs() const noexcept
    {
        return CacheAccntRecs(impl_);
    }
};

} // dbr

#endif // DBRPP_CACHE_HPP
