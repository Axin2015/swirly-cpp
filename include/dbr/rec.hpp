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
#include <dbr/types.hpp>

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

class RecBase {
protected:
    const DbrRec& rec_;
    // Non-virtual is protected.
    ~RecBase() noexcept
    {
    }
    explicit
    RecBase(const DbrRec& rec) noexcept
        : rec_(rec)
    {
    }
public:
    bool
    operator ==(const RecBase& rhs) const noexcept
    {
        return rec_.type == rhs.rec_.type && rec_.id == rhs.rec_.id;
    }
    bool
    operator !=(const RecBase& rhs) const noexcept
    {
        return !(*this == rhs);
    }
    int
    type() const noexcept
    {
        return rec_.type;
    }
    DbrIden
    id() const noexcept
    {
        return rec_.id;
    }
    Mnem
    mnem() const noexcept
    {
        return Mnem(rec_.mnem);
    }
};

class InstrRec : public RecBase {
public:
    explicit
    InstrRec(const DbrRec& rec) noexcept
    : RecBase(rec)
    {
    }
    Display
    display() const noexcept
    {
        return Display(rec_.instr.display);
    }
    Mnem
    asset_type() const noexcept
    {
        return Mnem(rec_.instr.asset_type);
    }
    Mnem
    instr_type() const noexcept
    {
        return Mnem(rec_.instr.instr_type);
    }
    Mnem
    asset() const noexcept
    {
        return Mnem(rec_.instr.asset);
    }
    Mnem
    ccy() const noexcept
    {
        return Mnem(rec_.instr.ccy);
    }
    int
    tick_numer() const noexcept
    {
        return rec_.instr.tick_numer;
    }
    int
    tick_denom() const noexcept
    {
        return rec_.instr.tick_denom;
    }
    double
    price_inc() const noexcept
    {
        return rec_.instr.price_inc;
    }
    int
    lot_numer() const noexcept
    {
        return rec_.instr.lot_numer;
    }
    int
    lot_denom() const noexcept
    {
        return rec_.instr.lot_denom;
    }
    double
    qty_inc() const noexcept
    {
        return rec_.instr.qty_inc;
    }
    int
    price_dp() const noexcept
    {
        return rec_.instr.price_dp;
    }
    int
    pip_dp() const noexcept
    {
        return rec_.instr.pip_dp;
    }
    int
    qty_dp() const noexcept
    {
        return rec_.instr.qty_dp;
    }
    DbrLots
    min_lots() const noexcept
    {
        return rec_.instr.min_lots;
    }
    DbrLots
    max_lots() const noexcept
    {
        return rec_.instr.max_lots;
    }
};

inline std::ostream&
operator <<(std::ostream& os, InstrRec irec)
{
    return os << "id=" << irec.id()
              << ",mnem=" << irec.mnem()
              << ",display=" << irec.display()
              << ",asset_type=" << irec.asset_type()
              << ",instr_type=" << irec.instr_type()
              << ",asset=" << irec.asset()
              << ",ccy=" << irec.ccy()
              << ",tick_numer=" << irec.tick_numer()
              << ",tick_denom=" << irec.tick_denom()
              << ",price_inc=" << irec.price_inc()
              << ",lot_numer=" << irec.lot_numer()
              << ",lot_denom=" << irec.lot_denom()
              << ",qty_inc=" << irec.qty_inc()
              << ",price_dp=" << irec.price_dp()
              << ",pip_dp=" << irec.pip_dp()
              << ",qty_dp=" << irec.qty_dp()
              << ",min_lots=" << irec.min_lots()
              << ",max_lots=" << irec.max_lots();
}

class MarketRec : public RecBase {
public:
    explicit
    MarketRec(const DbrRec& rec) noexcept
    : RecBase(rec)
    {
    }
    InstrRec
    irec() const noexcept
    {
        return InstrRec(*rec_.market.instr.rec);
    }
    Tenor
    tenor() const noexcept
    {
        return rec_.market.tenor;
    }
    DbrDate
    settl_date() const noexcept
    {
        return rec_.market.settl_date;
    }
};

inline std::ostream&
operator <<(std::ostream& os, MarketRec mrec)
{
    return os << "id=" << mrec.id()
              << ",mnem=" << mrec.mnem()
              << ",instr=" << mrec.irec().mnem()
              << ",tenor=" << mrec.tenor()
              << ",settl_date=" << mrec.settl_date();
}

class TraderRec : public RecBase {
public:
    explicit
    TraderRec(const DbrRec& rec) noexcept
    : RecBase(rec)
    {
    }
    Display
    display() const noexcept
    {
        return Display(rec_.trader.display);
    }
    Email
    email() const noexcept
    {
        return Email(rec_.trader.email);
    }
};

inline std::ostream&
operator <<(std::ostream& os, TraderRec trec)
{
    return os << "id=" << trec.id()
              << ",mnem=" << trec.mnem()
              << ",display=" << trec.display()
              << ",email=" << trec.email();
}

class AccntRec : public RecBase {
public:
    explicit
    AccntRec(const DbrRec& rec) noexcept
    : RecBase(rec)
    {
    }
    Display
    display() const noexcept
    {
        return Display(rec_.accnt.display);
    }
    Email
    email() const noexcept
    {
        return Email(rec_.accnt.email);
    }
};

inline std::ostream&
operator <<(std::ostream& os, AccntRec arec)
{
    return os << "id=" << arec.id()
              << ",mnem=" << arec.mnem()
              << ",display=" << arec.display()
              << ",email=" << arec.email();
}

} // dbr

#endif // DBR_REC_HPP
