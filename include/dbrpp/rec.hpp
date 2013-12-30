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

#include <dbrpp/types.hpp>

#include <dbr/conv.h>
#include <dbr/text.h>

namespace dbr {

/**
 * @addtogroup TypesRecData
 * @{
 */

class RecRefBase {
protected:
    DbrRec* impl_;
    // Non-virtual is protected.
    ~RecRefBase() noexcept
    {
    }
    explicit
    RecRefBase(DbrRec& impl) noexcept
        : impl_{&impl}
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
        return Mnem{impl_->mnem};
    }
    Display
    display() const noexcept
    {
        return Display{impl_->display};
    }
};

class RecRef : public RecRefBase {
public:
    explicit
    RecRef(DbrRec& impl) noexcept
        : RecRefBase{impl}
    {
    }
};

inline std::ostream&
operator <<(std::ostream& os, RecRef trec)
{
    return os << "id=" << trec.id()
              << ",mnem=" << trec.mnem()
              << ",display=" << trec.display();
}

class TraderRecRef : public RecRefBase {
public:
    explicit
    TraderRecRef(DbrRec& impl) noexcept
        : RecRefBase{impl}
    {
    }
    Email
    email() const noexcept
    {
        return Email{impl_->trader.email};
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

class AccntRecRef : public RecRefBase {
public:
    explicit
    AccntRecRef(DbrRec& impl) noexcept
        : RecRefBase{impl}
    {
    }
    Email
    email() const noexcept
    {
        return Email{impl_->accnt.email};
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

class ContrRecRef : public RecRefBase {
public:
    explicit
    ContrRecRef(DbrRec& impl) noexcept
        : RecRefBase{impl}
    {
    }
    Mnem
    asset_type() const noexcept
    {
        return Mnem{impl_->contr.asset_type};
    }
    Mnem
    asset() const noexcept
    {
        return Mnem{impl_->contr.asset};
    }
    Mnem
    ccy() const noexcept
    {
        return Mnem{impl_->contr.ccy};
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
    DbrPriceText
    price_text(double price) const noexcept
    {
        DbrPriceText pt;
        dbr_price_text(price, price_dp(), pip_dp(), &pt);
        return pt;
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
              << ",lot_numer=" << crec.lot_numer()
              << ",lot_denom=" << crec.lot_denom()
              << ",pip_dp=" << crec.pip_dp()
              << ",min_lots=" << crec.min_lots()
              << ",max_lots=" << crec.max_lots();
}

/** @} */

} // dbr

#endif // DBRPP_REC_HPP
