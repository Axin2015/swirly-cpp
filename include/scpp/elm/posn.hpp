/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_POSN_HPP
#define SCPP_ELM_POSN_HPP

#include <scpp/elm/rec.hpp>

namespace sc {

/**
 * @addtogroup Posn
 * @{
 */

class PosnRef {
    ScPosn* impl_;
public:
    explicit
    PosnRef(ScPosn& impl) noexcept
        : impl_{&impl}
    {
    }
    operator ScPosn&() const noexcept
    {
        return *impl_;
    }
    ScPosn*
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
    ScKey
    key() const noexcept
    {
        return sc_posn_key(impl_->accnt.rec->id, impl_->contr.rec->id, impl_->settl_day);
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
    ScJd
    settl_day() const noexcept
    {
        return impl_->settl_day;
    }
    ScLicks
    buy_licks() const noexcept
    {
        return impl_->buy_licks;
    }
    ScLots
    buy_lots() const noexcept
    {
        return impl_->buy_lots;
    }
    ScLicks
    sell_licks() const noexcept
    {
        return impl_->sell_licks;
    }
    ScLots
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

} // sc

#endif // SCPP_ELM_POSN_HPP
