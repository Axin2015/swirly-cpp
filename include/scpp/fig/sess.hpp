/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_FIG_SESS_HPP
#define SCPP_FIG_SESS_HPP

#include <scpp/elm/types.hpp>

#include <scpp/ash/except.hpp>
#include <scpp/ash/iter.hpp>
#include <scpp/ash/rbnode.hpp>

#include <sc/fig/sess.h>

#include <iostream>
#include <limits>

#include <uuid/uuid.h>

namespace sc {

/**
 * @addtogroup Sess
 * @{
 */

class SessAccnts {
    struct Policy : NodeTraits<ScRbNode> {
        typedef FigAccnt Entry;
        static Entry*
        entry(Node* node)
        {
            return sc_sess_accnt_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return sc_sess_accnt_entry(const_cast<Node*>(node));
        }
    };
    ScSess* sess_;
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
    SessAccnts(ScSess& sess) noexcept
        : sess_{&sess}
    {
    }
    void
    swap(SessAccnts& rhs) noexcept
    {
        std::swap(sess_, rhs.sess_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return sc_sess_first_accnt(sess_);
    }
    ConstIterator
    begin() const noexcept
    {
        return sc_sess_first_accnt(sess_);
    }
    Iterator
    end() noexcept
    {
        return SC_SESS_END_ACCNT;
    }
    ConstIterator
    end() const noexcept
    {
        return SC_SESS_END_ACCNT;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return sc_sess_last_accnt(sess_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return sc_sess_last_accnt(sess_);
    }
    ReverseIterator
    rend() noexcept
    {
        return SC_SESS_END_ACCNT;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return SC_SESS_END_ACCNT;
    }

    // Find.

    Iterator
    find(ScTicks ticks) noexcept
    {
        return sc_sess_find_accnt(sess_, ticks);
    }
    ConstIterator
    find(ScTicks ticks) const noexcept
    {
        return sc_sess_find_accnt(sess_, ticks);
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
        return sc_sess_empty_accnt(sess_);
    }
};

class Sess {
    mutable ScSess impl_;
public:
    ~Sess() noexcept
    {
        sc_sess_term(&impl_);
    }
    Sess(const ScUuid uuid, ScPool pool) noexcept
    {
        sc_sess_init(&impl_, uuid, pool);
    }
    operator ScSess&() noexcept
    {
        return impl_;
    }
    ScSess*
    c_arg() noexcept
    {
        return &impl_;
    }

    // Copy semantics.

    Sess(const Sess&) = delete;

    Sess&
    operator =(const Sess&) = delete;

    const unsigned char*
    uuid() const noexcept
    {
        return impl_.uuid;
    }
    SessAccnts
    accnts() const noexcept
    {
        return SessAccnts{impl_};
    }
};

inline std::ostream&
operator <<(std::ostream& os, const Sess& sess)
{
    char buf[SC_UUID_MAX + 1];
    uuid_unparse_lower(sess.uuid(), buf);
    return os << "uuid=" << buf;
}

class SessRef {
    ScSess* impl_;
public:
    explicit
    SessRef(ScSess& impl) noexcept
        : impl_{&impl}
    {
    }
    operator ScSess&() const noexcept
    {
        return *impl_;
    }
    ScSess*
    c_arg() const noexcept
    {
        return impl_;
    }
    const unsigned char*
    uuid() const noexcept
    {
        return impl_->uuid;
    }
    ScBool
    logon(ScAccnt accnt)
    {
        return sc_sess_logon(impl_, accnt);
    }
    void
    logoff(ScAccnt accnt)
    {
        return sc_sess_logoff(impl_, accnt);
    }
    void
    logoff_and_reset(ScAccnt accnt)
    {
        return sc_sess_logoff_and_reset(impl_, accnt);
    }
    int
    subs(ScAccnt accnt) const noexcept
    {
        return sc_sess_subs(impl_, accnt);
    }
    SessAccnts
    accnts() const noexcept
    {
        return SessAccnts{*impl_};
    }
};

inline std::ostream&
operator <<(std::ostream& os, SessRef sess)
{
    char buf[SC_UUID_MAX + 1];
    uuid_unparse_lower(sess.uuid(), buf);
    return os << "uuid=" << buf;
}

/** @} */

} // sc

#endif // SCPP_FIG_SESS_HPP
