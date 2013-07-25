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
#include <dbr/msg.h>
#include <dbr/string.h>

namespace dbr {

class RecBase {
protected:
    DbrRec& impl_;
    // Non-virtual is protected.
    ~RecBase() noexcept
    {
    }
    explicit
    RecBase(DbrRec& impl) noexcept
        : impl_(impl)
    {
    }
public:
    operator DbrRec&() const noexcept
    {
        return impl_;
    }
    DbrRec*
    c_arg() noexcept
    {
        return &impl_;
    }
    bool
    operator ==(const RecBase& rhs) const noexcept
    {
        return impl_.type == rhs.impl_.type && impl_.id == rhs.impl_.id;
    }
    bool
    operator !=(const RecBase& rhs) const noexcept
    {
        return !(*this == rhs);
    }
    int
    type() const noexcept
    {
        return impl_.type;
    }
    DbrIden
    id() const noexcept
    {
        return impl_.id;
    }
    Mnem
    mnem() const noexcept
    {
        return Mnem(impl_.mnem);
    }
};

class InstrRec : public RecBase {
public:
    explicit
    InstrRec(DbrRec& impl) noexcept
    : RecBase(impl)
    {
    }
    Display
    display() const noexcept
    {
        return Display(impl_.instr.display);
    }
    Mnem
    asset_type() const noexcept
    {
        return Mnem(impl_.instr.asset_type);
    }
    Mnem
    instr_type() const noexcept
    {
        return Mnem(impl_.instr.instr_type);
    }
    Mnem
    asset() const noexcept
    {
        return Mnem(impl_.instr.asset);
    }
    Mnem
    ccy() const noexcept
    {
        return Mnem(impl_.instr.ccy);
    }
    int
    tick_numer() const noexcept
    {
        return impl_.instr.tick_numer;
    }
    int
    tick_denom() const noexcept
    {
        return impl_.instr.tick_denom;
    }
    double
    price_inc() const noexcept
    {
        return impl_.instr.price_inc;
    }
    int
    lot_numer() const noexcept
    {
        return impl_.instr.lot_numer;
    }
    int
    lot_denom() const noexcept
    {
        return impl_.instr.lot_denom;
    }
    double
    qty_inc() const noexcept
    {
        return impl_.instr.qty_inc;
    }
    int
    price_dp() const noexcept
    {
        return impl_.instr.price_dp;
    }
    int
    pip_dp() const noexcept
    {
        return impl_.instr.pip_dp;
    }
    int
    qty_dp() const noexcept
    {
        return impl_.instr.qty_dp;
    }
    DbrLots
    min_lots() const noexcept
    {
        return impl_.instr.min_lots;
    }
    DbrLots
    max_lots() const noexcept
    {
        return impl_.instr.max_lots;
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

inline char*
write_entity(char* buf, InstrRec irec)
{
    buf = dbr_write_instr(buf, irec.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

inline const char*
read_entity(const char* buf, InstrRec irec)
{
    buf = dbr_read_instr(buf, irec.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

inline int
entity_len(InstrRec irec)
{
    const int len = dbr_instr_len(irec.c_arg());
    if (len < 0)
        throw_exception();
    return len;
}

class MarketRec : public RecBase {
public:
    explicit
    MarketRec(DbrRec& impl) noexcept
    : RecBase(impl)
    {
    }
    InstrRec
    irec() const noexcept
    {
        return InstrRec(*impl_.market.instr.rec);
    }
    Tenor
    tenor() const noexcept
    {
        return impl_.market.tenor;
    }
    DbrDate
    settl_date() const noexcept
    {
        return impl_.market.settl_date;
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

inline char*
write_entity(char* buf, MarketRec mrec)
{
    buf = dbr_write_market(buf, mrec.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

inline const char*
read_entity(const char* buf, MarketRec mrec)
{
    buf = dbr_read_market(buf, mrec.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

inline int
entity_len(MarketRec mrec)
{
    const int len = dbr_market_len(mrec.c_arg());
    if (len < 0)
        throw_exception();
    return len;
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
        return Display(impl_.trader.display);
    }
    Email
    email() const noexcept
    {
        return Email(impl_.trader.email);
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

inline char*
write_entity(char* buf, TraderRec trec)
{
    buf = dbr_write_trader(buf, trec.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

inline const char*
read_entity(const char* buf, TraderRec trec)
{
    buf = dbr_read_trader(buf, trec.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

inline int
entity_len(TraderRec trec)
{
    const int len = dbr_trader_len(trec.c_arg());
    if (len < 0)
        throw_exception();
    return len;
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
        return Display(impl_.accnt.display);
    }
    Email
    email() const noexcept
    {
        return Email(impl_.accnt.email);
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

inline char*
write_entity(char* buf, AccntRec arec)
{
    buf = dbr_write_accnt(buf, arec.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

inline const char*
read_entity(const char* buf, AccntRec arec)
{
    buf = dbr_read_accnt(buf, arec.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

inline int
entity_len(AccntRec arec)
{
    const int len = dbr_accnt_len(arec.c_arg());
    if (len < 0)
        throw_exception();
    return len;
}

} // dbr

#endif // DBRPP_REC_HPP
