/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_MODEL_HPP
#define SCPP_ELM_MODEL_HPP

#include <scpp/ash/except.hpp>
#include <scpp/ash/iter.hpp>
#include <scpp/ash/slnode.hpp>

#include <sc/elm/model.h>
#include <sc/elm/pool.h>

#include <limits> // numeric_limits<>
#include <memory> // unique_ptr<>

namespace sc {

/**
 * @addtogroup Model
 * @{
 */

struct ModelDelete {
    void
    operator()(ScModel model) const noexcept
    {
        sc_model_destroy(model);
    }
};

typedef std::unique_ptr<ScIModel, ModelDelete> ModelPtr;

template <int TypeN>
class ModelRecs {
    struct Policy : NodeTraits<ScSlNode> {
        typedef ScRec Entry;
        static Entry*
        entry(Node* node)
        {
            return sc_shared_rec_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return sc_shared_rec_entry(const_cast<Node*>(node));
        }
    };
    ScSlNode* first_;
    size_t size_;
    ScPool pool_;
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
        ScSlNode* node = first_;
        while (node) {
            ScRec* rec = sc_shared_rec_entry(node);
            node = node->next;
            sc_pool_free_rec(pool_, rec);
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
    ModelRecs(ScSlNode* first, size_t size, ScPool pool) noexcept
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

typedef ModelRecs<SC_ENTITY_ACCNT> ModelAccntRecs;
typedef ModelRecs<SC_ENTITY_CONTR> ModelContrRecs;

template <class DerivedT>
class IModel : public ScIModel {
    static void
    destroy(ScModel model) noexcept
    {
        return static_cast<DerivedT*>(model)->destroy();
    }
    static ssize_t
    read_entity(ScModel model, int type, ScPool pool, ScSlNode** first) noexcept
    {
        return static_cast<DerivedT*>(model)->read_entity(type, pool, *first);
    }
    static const ScModelVtbl*
    vtbl() noexcept
    {
        static const ScModelVtbl VTBL = {
            destroy,
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
        : ScIModel{vtbl()}
    {
    }
};

inline void
destroy(ScModel model)
{
    model->vtbl->destroy(model);
}

inline size_t
read_entity(ScModel model, int type, ScPool pool, ScSlNode*& first)
{
    const auto size = model->vtbl->read_entity(model, type, pool, &first);
    if (size < 0)
        throw_exception();
    return size;
}

template <int TypeN>
inline ModelRecs<TypeN>
read_entity(ScModel model, ScPool pool)
{
    ScSlNode* first;
    const auto size = read_entity(model, TypeN, pool, first);
    return ModelRecs<TypeN>{first, size, pool};
}

/** @} */

} // sc

#endif // SCPP_ELM_MODEL_HPP
