/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#ifndef DBRPP_MODEL_HPP
#define DBRPP_MODEL_HPP

#include <dbrpp/except.hpp>
#include <dbrpp/iter.hpp>
#include <dbrpp/slnode.hpp>

#include <dbr/model.h>

#include <limits> // numeric_limits<>

namespace dbr {

/**
 * @addtogroup Model
 * @{
 */

template <int TypeN>
class ModelRecs {
    struct Policy : NodeTraits<DbrSlNode> {
        typedef DbrRec Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_shared_rec_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_shared_rec_entry(const_cast<Node*>(node));
        }
    };
    DbrSlNode* first_;
    size_t size_;
    DbrPool pool_;
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
private:
    void
    destroy()
    {
        DbrSlNode* node = first_;
        while (node) {
            DbrRec* rec = dbr_shared_rec_entry(node);
            node = node->next;
            dbr_pool_free_rec(pool_, rec);
        }
    }
public:
    ~ModelRecs() noexcept
    {
        if (pool_)
            destroy();
    }
    constexpr
    ModelRecs(decltype(nullptr)) noexcept
        : first_{nullptr},
          size_{0},
          pool_{nullptr}
    {
    }
    ModelRecs(DbrSlNode* first, size_t size, DbrPool pool) noexcept
        : first_{first},
          size_{size},
          pool_{pool}
    {
    }
    // Copy semantics.

    ModelRecs(const ModelRecs&) = delete;

    ModelRecs&
    operator =(const ModelRecs&) = delete;

    // Move semantics.

    ModelRecs(ModelRecs&& rhs) noexcept
        : first_{nullptr},
          size_{0},
          pool_{nullptr}
    {
        swap(rhs);
    }
    ModelRecs&
    operator =(ModelRecs&& rhs) noexcept
    {
        if (pool_) {
            destroy();
            first_ = nullptr;
            size_ = 0;
            pool_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(ModelRecs& rhs) noexcept
    {
        std::swap(first_, rhs.first_);
        std::swap(size_, rhs.size_);
        std::swap(pool_, rhs.pool_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return first_;
    }
    ConstIterator
    begin() const noexcept
    {
        return first_;
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
        return size_;
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

typedef ModelRecs<DBR_ENTITY_TRADER> ModelTraderRecs;
typedef ModelRecs<DBR_ENTITY_ACCNT> ModelAccntRecs;
typedef ModelRecs<DBR_ENTITY_CONTR> ModelContrRecs;

template <class DerivedT>
class IModel : public DbrIModel {
    static ssize_t
    read_entity(DbrModel model, int type, DbrPool pool, DbrSlNode** first) noexcept
    {
        return static_cast<DerivedT*>(model)->read_entity(type, pool, *first);
    }
    static const DbrModelVtbl*
    vtbl() noexcept
    {
        static const DbrModelVtbl VTBL = {
            read_entity
        };
        return &VTBL;
    }
protected:
    ~IModel() noexcept
    {
    }
public:
    IModel() noexcept
        : DbrIModel{vtbl()}
    {
    }
};

inline size_t
read_entity(DbrModel model, int type, DbrPool pool, DbrSlNode*& first)
{
    const auto size = model->vtbl->read_entity(model, type, pool, &first);
    if (size < 0)
        throw_exception();
    return size;
}

template <int TypeN>
inline ModelRecs<TypeN>
read_entity(DbrModel model, DbrPool pool)
{
    DbrSlNode* first;
    const auto size = read_entity(model, TypeN, pool, first);
    return ModelRecs<TypeN>{first, size, pool};
}

/** @} */

} // dbr

#endif // DBRPP_MODEL_HPP
