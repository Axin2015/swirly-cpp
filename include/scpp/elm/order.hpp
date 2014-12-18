/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_ORDER_HPP
#define SCPP_ELM_ORDER_HPP

#include <scpp/elm/rec.hpp>

namespace sc {

/**
 * @addtogroup Order
 * @{
 */

class OrderRef {
    ScOrder* impl_;
public:
    explicit
    OrderRef(ScOrder& impl) noexcept
        : impl_{&impl}
    {
    }
    operator ScOrder&() const noexcept
    {
        return *impl_;
    }
    ScOrder*
    c_arg() const noexcept
    {
        return impl_;
    }
    bool
    operator ==(OrderRef rhs) const noexcept
    {
        return impl_->id == rhs.impl_->id;
    }
    bool
    operator !=(OrderRef rhs) const noexcept
    {
        return impl_->id != rhs.impl_->id;
    }
    ScIden
    id() const noexcept
    {
        return impl_->id;
    }
    AccntRecRef
    trec() const noexcept
    {
        return AccntRecRef{*impl_->i.trader.rec};
    }
    AccntRecRef
    grec() const noexcept
    {
        return AccntRecRef{*impl_->i.giveup.rec};
    }
    ContrRecRef
    crec() const noexcept
    {
        return ContrRecRef{*impl_->i.contr.rec};
    }
    ScJd
    settl_day() const noexcept
    {
        return impl_->i.settl_day;
    }
    StringRef
    ref() const noexcept
    {
        return StringRef{impl_->i.ref, MaxSize{SC_REF_MAX}};
    }
    int
    state() const noexcept
    {
        return impl_->i.state;
    }
    int
    action() const noexcept
    {
        return impl_->i.action;
    }
    ScTicks
    ticks() const noexcept
    {
        return impl_->i.ticks;
    }
    ScLots
    lots() const noexcept
    {
        return impl_->i.lots;
    }
    ScLots
    resd() const noexcept
    {
        return impl_->i.resd;
    }
    ScLots
    exec() const noexcept
    {
        return impl_->i.exec;
    }
    ScTicks
    last_ticks() const noexcept
    {
        return impl_->i.last_ticks;
    }
    ScLots
    last_lots() const noexcept
    {
        return impl_->i.last_lots;
    }
    ScLots
    min_lots() const noexcept
    {
        return impl_->i.min_lots;
    }
    ScMillis
    created() const noexcept
    {
        return impl_->created;
    }
    ScMillis
    modified() const noexcept
    {
        return impl_->modified;
    }
    bool
    done() const noexcept
    {
        return impl_->i.resd == 0;
    }
};

inline std::ostream&
operator <<(std::ostream& os, OrderRef order)
{
    return os << "id=" << order.id()
              << ",trader=" << order.trec().mnem()
              << ",giveup=" << order.grec().mnem()
              << ",contr=" << order.crec().mnem()
              << ",settl_day=" << order.settl_day()
              << ",ref=" << order.ref()
              << ",state=" << order.state()
              << ",action=" << order.action()
              << ",ticks=" << order.ticks()
              << ",lots=" << order.lots()
              << ",resd=" << order.resd()
              << ",exec=" << order.exec()
              << ",last_ticks=" << order.last_ticks()
              << ",last_lots=" << order.last_lots()
              << ",min_lots=" << order.min_lots()
              << ",created=" << order.created()
              << ",modified=" << order.modified();
}

/** @} */

} // sc

#endif // SCPP_ELM_ORDER_HPP
