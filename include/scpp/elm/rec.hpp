/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_REC_HPP
#define SCPP_ELM_REC_HPP

#include <scpp/elm/types.hpp>

#include <sc/elm/conv.h>
#include <sc/elm/format.h>

namespace sc {

/**
 * @addtogroup Rec
 * @{
 */

class RecRefBase {
protected:
    ScRec* impl_;
    // Non-virtual is protected.
    ~RecRefBase() noexcept
    {
    }
    explicit
    RecRefBase(ScRec& impl) noexcept
        : impl_{&impl}
    {
    }
public:
    operator ScRec&() const noexcept
    {
        return *impl_;
    }
    ScRec*
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
    ScIden
    id() const noexcept
    {
        return impl_->id;
    }
    StringRef
    mnem() const noexcept
    {
        return StringRef{impl_->mnem, MaxSize{SC_MNEM_MAX}};
    }
    StringRef
    display() const noexcept
    {
        return StringRef{impl_->display, MaxSize{SC_DISPLAY_MAX}};
    }
};

class RecRef : public RecRefBase {
public:
    explicit
    RecRef(ScRec& impl) noexcept
        : RecRefBase{impl}
    {
    }
};

inline std::ostream&
operator <<(std::ostream& os, RecRef rec)
{
    return os << "id=" << rec.id()
              << ",mnem=" << rec.mnem()
              << ",display=" << rec.display();
}

/** @} */

/**
 * @addtogroup AccntRec
 * @{
 */

class AccntRecRef : public RecRefBase {
public:
    explicit
    AccntRecRef(ScRec& impl) noexcept
        : RecRefBase{impl}
    {
    }
    StringRef
    email() const noexcept
    {
        return StringRef{impl_->accnt.email, MaxSize{SC_EMAIL_MAX}};
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

/** @} */

/**
 * @addtogroup ContrRec
 * @{
 */

class ContrRecRef : public RecRefBase {
public:
    explicit
    ContrRecRef(ScRec& impl) noexcept
        : RecRefBase{impl}
    {
    }
    StringRef
    asset_type() const noexcept
    {
        return StringRef{impl_->contr.asset_type, MaxSize{SC_MNEM_MAX}};
    }
    StringRef
    asset() const noexcept
    {
        return StringRef{impl_->contr.asset, MaxSize{SC_MNEM_MAX}};
    }
    StringRef
    ccy() const noexcept
    {
        return StringRef{impl_->contr.ccy, MaxSize{SC_MNEM_MAX}};
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
    ScLots
    min_lots() const noexcept
    {
        return impl_->contr.min_lots;
    }
    ScLots
    max_lots() const noexcept
    {
        return impl_->contr.max_lots;
    }
    // Conversion functions.
    ScLots
    qty_to_lots(double qty) const noexcept
    {
        return sc_real_to_incs(qty, qty_inc());
    }
    double
    lots_to_qty(ScLots lots) const noexcept
    {
        return sc_incs_to_real(lots, qty_inc());
    }
    ScTicks
    price_to_ticks(double price) const noexcept
    {
        return sc_real_to_incs(price, price_inc());
    }
    double
    ticks_to_price(ScTicks ticks) const noexcept
    {
        return sc_incs_to_real(ticks, price_inc());
    }
    ScPriceString
    price_string(double price) const noexcept
    {
        ScPriceString ps;
        sc_format_price(price, price_dp(), pip_dp(), &ps);
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
              << ",lot_numer=" << crec.lot_numer()
              << ",lot_denom=" << crec.lot_denom()
              << ",pip_dp=" << crec.pip_dp()
              << ",min_lots=" << crec.min_lots()
              << ",max_lots=" << crec.max_lots();
}

/** @} */

} // sc

#endif // SCPP_ELM_REC_HPP
