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

class RecBase {
protected:
    DbrRec* impl_;
    // Non-virtual is protected.
    ~RecBase() noexcept
    {
    }
    explicit
    RecBase(DbrRec& impl) noexcept
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
    operator ==(const RecBase& rhs) const noexcept
    {
        return impl_->type == rhs.impl_->type && impl_->id == rhs.impl_->id;
    }
    bool
    operator !=(const RecBase& rhs) const noexcept
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
};

class ContrRec : public RecBase {
public:
    explicit
    ContrRec(DbrRec& impl) noexcept
    : RecBase(impl)
    {
    }
    Display
    display() const noexcept
    {
        return Display(impl_->contr.display);
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
operator <<(std::ostream& os, ContrRec crec)
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
entity_len(ContrRec crec) noexcept
{
    return dbr_contr_len(crec.c_arg());
}

inline char*
write_entity(char* buf, ContrRec crec) noexcept
{
    return dbr_write_contr(buf, crec.c_arg());
}

inline const char*
read_entity(const char* buf, ContrRec crec)
{
    buf = dbr_read_contr(buf, crec.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

class TraderRec : public RecBase {
public:
    explicit
    TraderRec(DbrRec& impl) noexcept
    : RecBase(impl)
    {
    }
    Display
    display() const noexcept
    {
        return Display(impl_->trader.display);
    }
    Email
    email() const noexcept
    {
        return Email(impl_->trader.email);
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

inline size_t
entity_len(TraderRec trec) noexcept
{
    return dbr_trader_len(trec.c_arg());
}

inline char*
write_entity(char* buf, TraderRec trec) noexcept
{
    return dbr_write_trader(buf, trec.c_arg());
}

inline const char*
read_entity(const char* buf, TraderRec trec)
{
    buf = dbr_read_trader(buf, trec.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

class AccntRec : public RecBase {
public:
    explicit
    AccntRec(DbrRec& impl) noexcept
    : RecBase(impl)
    {
    }
    Display
    display() const noexcept
    {
        return Display(impl_->accnt.display);
    }
    Email
    email() const noexcept
    {
        return Email(impl_->accnt.email);
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

inline size_t
entity_len(AccntRec arec) noexcept
{
    return dbr_accnt_len(arec.c_arg());
}

inline char*
write_entity(char* buf, AccntRec arec) noexcept
{
    return dbr_write_accnt(buf, arec.c_arg());
}

inline const char*
read_entity(const char* buf, AccntRec arec)
{
    buf = dbr_read_accnt(buf, arec.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

} // dbr

#endif // DBRPP_REC_HPP
