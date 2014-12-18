/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_FIG_CLNT_HPP
#define SCPP_FIG_CLNT_HPP

#include <scpp/fig/accnt.hpp>

#include <scpp/elm/order.hpp>

#include <scpp/ash/except.hpp>
#include <scpp/ash/slnode.hpp>

#include <sc/fig/clnt.h>

namespace sc {

/**
 * @addtogroup Clnt
 * @{
 */

template <int TypeN>
class ClntRecs {
    struct Policy : NodeTraits<ScSlNode> {
        typedef ScRec Entry;
        static Entry*
        entry(Node* node)
        {
            return sc_clnt_rec_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return sc_clnt_rec_entry(const_cast<Node*>(node));
        }
    };
    ScClnt clnt_;
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
    ClntRecs(ScClnt clnt) noexcept
        : clnt_{clnt}
    {
    }
    void
    swap(ClntRecs& rhs) noexcept
    {
        std::swap(clnt_, rhs.clnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return sc_clnt_first_rec(clnt_, TypeN, nullptr);
    }
    ConstIterator
    begin() const noexcept
    {
        return sc_clnt_first_rec(clnt_, TypeN, nullptr);
    }
    Iterator
    end() noexcept
    {
        return SC_CLNT_END_REC;
    }
    ConstIterator
    end() const noexcept
    {
        return SC_CLNT_END_REC;
    }
    Iterator
    find(ScIden id) noexcept
    {
        return sc_clnt_find_rec_id(clnt_, TypeN, id);
    }
    ConstIterator
    find(ScIden id) const noexcept
    {
        return sc_clnt_find_rec_id(clnt_, TypeN, id);
    }
    Iterator
    find(const char* mnem) noexcept
    {
        return sc_clnt_find_rec_mnem(clnt_, TypeN, mnem);
    }
    ConstIterator
    find(const char* mnem) const noexcept
    {
        return sc_clnt_find_rec_mnem(clnt_, TypeN, mnem);
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
        sc_clnt_first_rec(clnt_, TypeN, &size);
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
        return sc_clnt_empty_rec(clnt_, TypeN) == SC_TRUE;
    }
};

typedef ClntRecs<SC_ENTITY_ACCNT> ClntAccntRecs;
typedef ClntRecs<SC_ENTITY_CONTR> ClntContrRecs;

class ClntViews {
    struct Policy : NodeTraits<ScRbNode> {
        typedef ScView Entry;
        static Entry*
        entry(Node* node)
        {
            return sc_clnt_view_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return sc_clnt_view_entry(const_cast<Node*>(node));
        }
    };
    ScClnt clnt_;
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
    ClntViews(ScClnt clnt) noexcept
    : clnt_{clnt}
    {
    }
    void
    swap(ClntViews& rhs) noexcept
    {
        std::swap(clnt_, rhs.clnt_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return sc_clnt_first_view(clnt_);
    }
    ConstIterator
    begin() const noexcept
    {
        return sc_clnt_first_view(clnt_);
    }
    Iterator
    end() noexcept
    {
        return SC_CLNT_END_VIEW;
    }
    ConstIterator
    end() const noexcept
    {
        return SC_CLNT_END_VIEW;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return sc_clnt_last_view(clnt_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return sc_clnt_last_view(clnt_);
    }
    ReverseIterator
    rend() noexcept
    {
        return SC_CLNT_END_VIEW;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return SC_CLNT_END_VIEW;
    }

    // Find.

    Iterator
    find(ScIden cid, ScJd settl_day) noexcept
    {
        return sc_clnt_find_view(clnt_, cid, settl_day);
    }
    ConstIterator
    find(ScIden cid, ScJd settl_day) const noexcept
    {
        return sc_clnt_find_view(clnt_, cid, settl_day);
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
        return sc_clnt_empty_view(clnt_);
    }
};

class Clnt {
    ScClnt impl_;
public:
    ~Clnt() noexcept
    {
        if (impl_)
            sc_clnt_destroy(impl_);
    }
    constexpr
    Clnt(decltype(nullptr)) noexcept
        : impl_{nullptr}
    {
    }
    Clnt(void* zctx, const ScUuid uuid, const char* mdaddr, const char* traddr,
         ScMillis tmout, ScPool pool)
        : impl_{sc_clnt_create(zctx, uuid, mdaddr, traddr, tmout, pool)}
    {
        if (!impl_)
            throw_exception();
    }
    operator ScClnt() const noexcept
    {
        return impl_;
    }

    // Copy semantics.

    Clnt(const Clnt&) = delete;

    Clnt&
    operator =(const Clnt&) = delete;

    // Move semantics.

    Clnt(Clnt&& rhs) noexcept
        : impl_{nullptr}
    {
        swap(rhs);
    }
    Clnt&
    operator =(Clnt&& rhs) noexcept
    {
        if (impl_) {
            sc_clnt_destroy(impl_);
            impl_ = nullptr;
        }
        swap(rhs);
        return *this;
    }
    void
    swap(Clnt& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    void
    reset() noexcept
    {
        sc_clnt_reset(impl_);
    }
    ScIden
    close()
    {
        ScIden req_id = sc_clnt_close(impl_);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    template <int TypeN>
    ClntRecs<TypeN>
    recs() const noexcept
    {
        return ClntRecs<TypeN>{impl_};
    }
    ClntAccntRecs
    arecs() const noexcept
    {
        return ClntAccntRecs{impl_};
    }
    ClntContrRecs
    crecs() const noexcept
    {
        return ClntContrRecs{impl_};
    }
    Accnt
    accnt(ScRec& arec) const
    {
        ScAccnt accnt = sc_clnt_accnt(impl_, &arec);
        if (!accnt)
            throw_exception();
        return Accnt{accnt};
    }
    ScIden
    logon(ScAccnt accnt)
    {
        ScIden req_id = sc_clnt_logon(impl_, accnt);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    logoff(ScAccnt accnt)
    {
        ScIden req_id = sc_clnt_logoff(impl_, accnt);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    place(ScAccnt trader, ScAccnt giveup, ScRec& crec, ScJd settl_day, const char* ref,
          int action, ScTicks ticks, ScLots lots, ScLots min_lots)
    {
        ScIden req_id = sc_clnt_place(impl_, trader, giveup, &crec, settl_day, ref, action,
                                        ticks, lots, min_lots);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    revise(ScAccnt trader, ScIden id, ScLots lots)
    {
        ScIden req_id = sc_clnt_revise_id(impl_, trader, id, lots);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    revise(ScAccnt trader, const char* ref, ScLots lots)
    {
        ScIden req_id = sc_clnt_revise_ref(impl_, trader, ref, lots);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    cancel(ScAccnt trader, ScIden id)
    {
        ScIden req_id = sc_clnt_cancel_id(impl_, trader, id);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    cancel(ScAccnt trader, const char* ref)
    {
        ScIden req_id = sc_clnt_cancel_ref(impl_, trader, ref);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    ack_trade(ScAccnt trader, ScIden id)
    {
        ScIden req_id = sc_clnt_ack_trade(impl_, trader, id);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    set_timer(ScMillis absms)
    {
        const ScIden id = sc_clnt_set_timer(impl_, absms);
        if (id < 0)
            throw_exception();
        return id;
    }
    void
    cancel_timer(ScIden id)
    {
        sc_clnt_cancel_timer(impl_, id);
    }
    ClntViews
    views() const noexcept
    {
        return ClntViews{impl_};
    }
    const unsigned char*
    uuid() const noexcept
    {
        return sc_clnt_uuid(impl_);
    }
    bool
    is_closed() const noexcept
    {
        return sc_clnt_is_closed(impl_) == SC_TRUE;
    }
    bool
    is_ready() const noexcept
    {
        return sc_clnt_is_ready(impl_) == SC_TRUE;
    }
};

inline std::ostream&
operator <<(std::ostream& os, const Clnt& clnt)
{
    char buf[SC_UUID_MAX + 1];
    uuid_unparse_lower(clnt.uuid(), buf);
    return os << "uuid=" << buf;
}

class ClntRef {
    ScClnt impl_;
public:
    explicit
    ClntRef(ScClnt impl) noexcept
        : impl_{impl}
    {
    }
    operator ScClnt() const noexcept
    {
        return impl_;
    }
    void
    swap(ClntRef& rhs) noexcept
    {
        std::swap(impl_, rhs.impl_);
    }
    void
    reset() noexcept
    {
        sc_clnt_reset(impl_);
    }
    ScIden
    close()
    {
        ScIden req_id = sc_clnt_close(impl_);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    template <int TypeN>
    ClntRecs<TypeN>
    recs() const noexcept
    {
        return ClntRecs<TypeN>{impl_};
    }
    ClntAccntRecs
    arecs() const noexcept
    {
        return ClntAccntRecs{impl_};
    }
    ClntContrRecs
    crecs() const noexcept
    {
        return ClntContrRecs{impl_};
    }
    Accnt
    accnt(ScRec& arec) const
    {
        ScAccnt accnt = sc_clnt_accnt(impl_, &arec);
        if (!accnt)
            throw_exception();
        return Accnt{accnt};
    }
    ScIden
    logon(ScAccnt accnt)
    {
        ScIden req_id = sc_clnt_logon(impl_, accnt);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    logoff(ScAccnt accnt)
    {
        ScIden req_id = sc_clnt_logoff(impl_, accnt);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    place(ScAccnt trader, ScAccnt giveup, ScRec& crec, ScJd settl_day, const char* ref,
          int action, ScTicks ticks, ScLots lots, ScLots min_lots)
    {
        ScIden req_id = sc_clnt_place(impl_, trader, giveup, &crec, settl_day, ref, action,
                                        ticks, lots, min_lots);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    revise(ScAccnt trader, ScIden id, ScLots lots)
    {
        ScIden req_id = sc_clnt_revise_id(impl_, trader, id, lots);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    revise(ScAccnt trader, const char* ref, ScLots lots)
    {
        ScIden req_id = sc_clnt_revise_ref(impl_, trader, ref, lots);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    cancel(ScAccnt trader, ScIden id)
    {
        ScIden req_id = sc_clnt_cancel_id(impl_, trader, id);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    cancel(ScAccnt trader, const char* ref)
    {
        ScIden req_id = sc_clnt_cancel_ref(impl_, trader, ref);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    ack_trade(ScAccnt trader, ScIden id)
    {
        ScIden req_id = sc_clnt_ack_trade(impl_, trader, id);
        if (req_id < 0)
            throw_exception();
        return req_id;
    }
    ScIden
    set_timer(ScMillis absms)
    {
        const ScIden id = sc_clnt_set_timer(impl_, absms);
        if (id < 0)
            throw_exception();
        return id;
    }
    void
    cancel_timer(ScIden id)
    {
        sc_clnt_cancel_timer(impl_, id);
    }
    ClntViews
    views() const noexcept
    {
        return ClntViews{impl_};
    }
    const unsigned char*
    uuid() const noexcept
    {
        return sc_clnt_uuid(impl_);
    }
    bool
    is_closed() const noexcept
    {
        return sc_clnt_is_closed(impl_) == SC_TRUE;
    }
    bool
    is_ready() const noexcept
    {
        return sc_clnt_is_ready(impl_) == SC_TRUE;
    }
};

inline std::ostream&
operator <<(std::ostream& os, ClntRef clnt)
{
    char buf[SC_UUID_MAX + 1];
    uuid_unparse_lower(clnt.uuid(), buf);
    return os << "uuid=" << buf;
}

/** @} */

} // sc

#endif // SCPP_FIG_CLNT_HPP
