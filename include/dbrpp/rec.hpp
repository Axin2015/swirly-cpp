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
#ifndef DBRPP_REC_HPP
#define DBRPP_REC_HPP

#include <dbrpp/best.hpp>
#include <dbrpp/except.hpp>
#include <dbrpp/types.hpp>

#include <dbr/conv.h>
#include <dbr/proto.h>
#include <dbr/string.h>

namespace dbr {

class RecRefBase {
protected:
    DbrRec* impl_;
    // Non-virtual is protected.
    ~RecRefBase() noexcept
    {
    }
    explicit
    RecRefBase(DbrRec& impl) noexcept
        : impl_(&impl)
    {
    }
public:
    operator DbrRec&() const noexcept
    {
        return *impl_;
    }
    DbrRec*
    c_arg() noexcept
    {
        return impl_;
    }
    bool
    operator ==(const RecRefBase& rhs) const noexcept
    {
        return impl_->type == rhs.impl_->type && impl_->id == rhs.impl_->id;
    }
    bool
    operator !=(const RecRefBase& rhs) const noexcept
    {
        return !(*this == rhs);
    }
    int
    type() const noexcept
    {
        return impl_->type;
    }
    DbrIden
    id() const noexcept
    {
        return impl_->id;
    }
    Mnem
    mnem() const noexcept
    {
        return Mnem(impl_->mnem);
    }
    Display
    display() const noexcept
    {
        return Display(impl_->display);
    }
};

class TraderRecRef : public RecRefBase {
public:
    explicit
    TraderRecRef(DbrRec& impl) noexcept
    : RecRefBase(impl)
    {
    }
    Email
    email() const noexcept
    {
        return Email(impl_->trader.email);
    }
};

inline std::ostream&
operator <<(std::ostream& os, TraderRecRef trec)
{
    return os << "id=" << trec.id()
              << ",mnem=" << trec.mnem()
              << ",display=" << trec.display()
              << ",email=" << trec.email();
}

inline size_t
trader_len(const DbrRec& trec) noexcept
{
    return dbr_trader_len(&trec);
}

inline char*
write_trader(char* buf, const DbrRec& trec) noexcept
{
    return dbr_write_trader(buf, &trec);
}

inline const char*
read_trader(const char* buf, DbrRec& trec)
{
    buf = dbr_read_trader(buf, &trec);
    if (!buf)
        throw_exception();
    return buf;
}

class AccntRecRef : public RecRefBase {
public:
    explicit
    AccntRecRef(DbrRec& impl) noexcept
    : RecRefBase(impl)
    {
    }
    Email
    email() const noexcept
    {
        return Email(impl_->accnt.email);
    }
};

inline std::ostream&
operator <<(std::ostream& os, AccntRecRef arec)
{
    return os << "id=" << arec.id()
              << ",mnem=" << arec.mnem()
              << ",display=" << arec.display()
              << ",email=" << arec.email();
}

inline size_t
accnt_len(const DbrRec& arec) noexcept
{
    return dbr_accnt_len(&arec);
}

inline char*
write_accnt(char* buf, const DbrRec& arec) noexcept
{
    return dbr_write_accnt(buf, &arec);
}

inline const char*
read_accnt(const char* buf, DbrRec& arec)
{
    buf = dbr_read_accnt(buf, &arec);
    if (!buf)
        throw_exception();
    return buf;
}

class ContrRecRef : public RecRefBase {
public:
    explicit
    ContrRecRef(DbrRec& impl) noexcept
    : RecRefBase(impl)
    {
    }
    Mnem
    asset_type() const noexcept
    {
        return Mnem(impl_->contr.asset_type);
    }
    Mnem
    asset() const noexcept
    {
        return Mnem(impl_->contr.asset);
    }
    Mnem
    ccy() const noexcept
    {
        return Mnem(impl_->contr.ccy);
    }
    int
    tick_numer() const noexcept
    {
        return impl_->contr.tick_numer;
    }
    int
    tick_denom() const noexcept
    {
        return impl_->contr.tick_denom;
    }
    double
    price_inc() const noexcept
    {
        return impl_->contr.price_inc;
    }
    int
    lot_numer() const noexcept
    {
        return impl_->contr.lot_numer;
    }
    int
    lot_denom() const noexcept
    {
        return impl_->contr.lot_denom;
    }
    double
    qty_inc() const noexcept
    {
        return impl_->contr.qty_inc;
    }
    int
    price_dp() const noexcept
    {
        return impl_->contr.price_dp;
    }
    int
    pip_dp() const noexcept
    {
        return impl_->contr.pip_dp;
    }
    int
    qty_dp() const noexcept
    {
        return impl_->contr.qty_dp;
    }
    DbrLots
    min_lots() const noexcept
    {
        return impl_->contr.min_lots;
    }
    DbrLots
    max_lots() const noexcept
    {
        return impl_->contr.max_lots;
    }
    // Conversion functions.
    DbrLots
    qty_to_lots(double qty) const noexcept
    {
        return dbr_real_to_incs(qty, qty_inc());
    }
    double
    lots_to_qty(DbrLots lots) const noexcept
    {
        return dbr_incs_to_real(lots, qty_inc());
    }
    DbrTicks
    price_to_ticks(double price) const noexcept
    {
        return dbr_real_to_incs(price, price_inc());
    }
    double
    ticks_to_price(DbrTicks ticks) const noexcept
    {
        return dbr_incs_to_real(ticks, price_inc());
    }
    DbrPriceString
    format_price(double price) const noexcept
    {
        DbrPriceString ps;
        dbr_format_price(price, price_dp(), pip_dp(), &ps);
        return ps;
    }
};

inline std::ostream&
operator <<(std::ostream& os, ContrRecRef crec)
{
    return os << "id=" << crec.id()
              << ",mnem=" << crec.mnem()
              << ",display=" << crec.display()
              << ",asset_type=" << crec.asset_type()
              << ",asset=" << crec.asset()
              << ",ccy=" << crec.ccy()
              << ",tick_numer=" << crec.tick_numer()
              << ",tick_denom=" << crec.tick_denom()
              << ",price_inc=" << crec.price_inc()
              << ",lot_numer=" << crec.lot_numer()
              << ",lot_denom=" << crec.lot_denom()
              << ",qty_inc=" << crec.qty_inc()
              << ",price_dp=" << crec.price_dp()
              << ",pip_dp=" << crec.pip_dp()
              << ",qty_dp=" << crec.qty_dp()
              << ",min_lots=" << crec.min_lots()
              << ",max_lots=" << crec.max_lots();
}

inline size_t
contr_len(const DbrRec& crec) noexcept
{
    return dbr_contr_len(&crec);
}

inline char*
write_contr(char* buf, const DbrRec& crec) noexcept
{
    return dbr_write_contr(buf, &crec);
}

inline const char*
read_contr(const char* buf, DbrRec& crec)
{
    buf = dbr_read_contr(buf, &crec);
    if (!buf)
        throw_exception();
    return buf;
}

} // dbr

#endif // DBRPP_REC_HPP
