/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ELM_POSN_HPP
#define DBRPP_ELM_POSN_HPP

#include <dbrpp/elm/rec.hpp>

namespace dbr {

/**
 * @addtogroup Posn
 * @{
 */

class PosnRef {
    DbrPosn* impl_;
public:
    explicit
    PosnRef(DbrPosn& impl) noexcept
        : impl_{&impl}
    {
    }
    operator DbrPosn&() const noexcept
    {
        return *impl_;
    }
    DbrPosn*
    c_arg() noexcept
    {
        return impl_;
    }
    bool
    operator ==(PosnRef rhs) const noexcept
    {
        return key() == rhs.key();
    }
    bool
    operator !=(PosnRef rhs) const noexcept
    {
        return key() != rhs.key();
    }
    DbrKey
    key() const noexcept
    {
        return dbr_posn_key(impl_->accnt.rec->id, impl_->contr.rec->id, impl_->settl_day);
    }
    AccntRecRef
    arec() const noexcept
    {
        return AccntRecRef{*impl_->accnt.rec};
    }
    ContrRecRef
    crec() const noexcept
    {
        return ContrRecRef{*impl_->contr.rec};
    }
    DbrJd
    settl_day() const noexcept
    {
        return impl_->settl_day;
    }
    DbrLicks
    buy_licks() const noexcept
    {
        return impl_->buy_licks;
    }
    DbrLots
    buy_lots() const noexcept
    {
        return impl_->buy_lots;
    }
    DbrLicks
    sell_licks() const noexcept
    {
        return impl_->sell_licks;
    }
    DbrLots
    sell_lots() const noexcept
    {
        return impl_->sell_lots;
    }
};

inline std::ostream&
operator <<(std::ostream& os, PosnRef posn)
{
    return os << "arec=" << posn.arec().mnem()
              << ",crec=" << posn.crec().mnem()
              << ",settl_day=" << posn.settl_day()
              << ",buy_licks=" << posn.buy_licks()
              << ",buy_lots=" << posn.buy_lots()
              << ",sell_licks=" << posn.sell_licks()
              << ",sell_lots=" << posn.sell_lots();
}

/** @} */

} // dbr

#endif // DBRPP_ELM_POSN_HPP
