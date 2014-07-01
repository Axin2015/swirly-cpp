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
#ifndef DBRPP_FIG_SESS_HPP
#define DBRPP_FIG_SESS_HPP

#include <dbrpp/elm/types.hpp>

#include <dbrpp/ash/except.hpp>
#include <dbrpp/ash/iter.hpp>
#include <dbrpp/ash/rbnode.hpp>

#include <dbr/fig/sess.h>

#include <iostream>
#include <limits>

#include <uuid/uuid.h>

namespace dbr {

/**
 * @addtogroup Sess
 * @{
 */

class SessAccnts {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef FigAccnt Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_sess_accnt_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_sess_accnt_entry(const_cast<Node*>(node));
        }
    };
    DbrSess* sess_;
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
    SessAccnts(DbrSess& sess) noexcept
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
        return dbr_sess_first_accnt(sess_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_sess_first_accnt(sess_);
    }
    Iterator
    end() noexcept
    {
        return DBR_SESS_END_ACCNT;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_SESS_END_ACCNT;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_sess_last_accnt(sess_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_sess_last_accnt(sess_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_SESS_END_ACCNT;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_SESS_END_ACCNT;
    }

    // Find.

    Iterator
    find(DbrTicks ticks) noexcept
    {
        return dbr_sess_find_accnt(sess_, ticks);
    }
    ConstIterator
    find(DbrTicks ticks) const noexcept
    {
        return dbr_sess_find_accnt(sess_, ticks);
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
        return dbr_sess_empty_accnt(sess_);
    }
};

class Sess {
    mutable DbrSess impl_;
public:
    ~Sess() noexcept
    {
        dbr_sess_term(&impl_);
    }
    Sess(const DbrUuid uuid, DbrPool pool) noexcept
    {
        dbr_sess_init(&impl_, uuid, pool);
    }
    operator DbrSess&() noexcept
    {
        return impl_;
    }
    DbrSess*
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
    char buf[DBR_UUID_MAX + 1];
    uuid_unparse_lower(sess.uuid(), buf);
    return os << "uuid=" << buf;
}

class SessRef {
    DbrSess* impl_;
public:
    explicit
    SessRef(DbrSess& impl) noexcept
        : impl_{&impl}
    {
    }
    operator DbrSess&() const noexcept
    {
        return *impl_;
    }
    DbrSess*
    c_arg() const noexcept
    {
        return impl_;
    }
    const unsigned char*
    uuid() const noexcept
    {
        return impl_->uuid;
    }
    DbrBool
    logon(DbrAccnt accnt)
    {
        return dbr_sess_logon(impl_, accnt);
    }
    void
    logoff(DbrAccnt accnt)
    {
        return dbr_sess_logoff(impl_, accnt);
    }
    void
    logoff_and_reset(DbrAccnt accnt)
    {
        return dbr_sess_logoff_and_reset(impl_, accnt);
    }
    int
    subs(DbrAccnt accnt) const noexcept
    {
        return dbr_sess_subs(impl_, accnt);
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
    char buf[DBR_UUID_MAX + 1];
    uuid_unparse_lower(sess.uuid(), buf);
    return os << "uuid=" << buf;
}

/** @} */

} // dbr

#endif // DBRPP_FIG_SESS_HPP
