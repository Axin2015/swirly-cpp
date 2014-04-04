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
#ifndef DBRPP_SESS_HPP
#define DBRPP_SESS_HPP

#include <dbrpp/except.hpp>
#include <dbrpp/iter.hpp>
#include <dbrpp/rbnode.hpp>
#include <dbrpp/types.hpp>

#include <dbr/sess.h>

#include <iostream>
#include <limits>

#include <uuid/uuid.h>

namespace dbr {

/**
 * @addtogroup SessTrader
 * @{
 */

class SessTraders {
    struct Policy : NodeTraits<DbrRbNode> {
        typedef FigTrader Entry;
        static Entry*
        entry(Node* node)
        {
            return dbr_sess_trader_entry(node);
        }
        static const Entry*
        entry(const Node* node)
        {
            return dbr_sess_trader_entry(const_cast<Node*>(node));
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
    SessTraders(DbrSess& sess) noexcept
        : sess_{&sess}
    {
    }
    void
    swap(SessTraders& rhs) noexcept
    {
        std::swap(sess_, rhs.sess_);
    }

    // Iterator.

    Iterator
    begin() noexcept
    {
        return dbr_sess_first_trader(sess_);
    }
    ConstIterator
    begin() const noexcept
    {
        return dbr_sess_first_trader(sess_);
    }
    Iterator
    end() noexcept
    {
        return DBR_SESS_END_TRADER;
    }
    ConstIterator
    end() const noexcept
    {
        return DBR_SESS_END_TRADER;
    }

    // ReverseIterator.

    ReverseIterator
    rbegin() noexcept
    {
        return dbr_sess_last_trader(sess_);
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return dbr_sess_last_trader(sess_);
    }
    ReverseIterator
    rend() noexcept
    {
        return DBR_SESS_END_TRADER;
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return DBR_SESS_END_TRADER;
    }

    // Find.

    Iterator
    find(DbrTicks ticks) noexcept
    {
        return dbr_sess_find_trader(sess_, ticks);
    }
    ConstIterator
    find(DbrTicks ticks) const noexcept
    {
        return dbr_sess_find_trader(sess_, ticks);
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
        return dbr_sess_empty_trader(sess_);
    }
};

/** @} */

/**
 * @addtogroup Sess
 * @{
 */

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
    /**
     * @addtogroup SessTrader
     * @{
     */
    SessTraders
    traders() const noexcept
    {
        return SessTraders{impl_};
    }
    /** @} */
};

inline std::ostream&
operator <<(std::ostream& os, const Sess& sess)
{
    return os << "uuid=" << sess.uuid();
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
    logon(DbrTrader trader)
    {
        return dbr_sess_logon(impl_, trader);
    }
    void
    logoff(DbrTrader trader, DbrBool clear)
    {
        return dbr_sess_logoff(impl_, trader, clear);
    }
    int
    subs(DbrAccnt accnt) const noexcept
    {
        return dbr_sess_subs(impl_, accnt);
    }
    /**
     * @addtogroup SessTrader
     * @{
     */
    SessTraders
    traders() const noexcept
    {
        return SessTraders{*impl_};
    }
    /** @} */
};

inline std::ostream&
operator <<(std::ostream& os, SessRef sess)
{
    char buf[DBR_UUID_MAX_ + 1];
    uuid_unparse_lower(sess.uuid(), buf);
    return os << "uuid=" << buf;
}

/** @} */

} // dbr

#endif // DBRPP_SESS_HPP
