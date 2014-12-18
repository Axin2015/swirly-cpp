/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBRPP_ELM_ORDER_HPP
#define DBRPP_ELM_ORDER_HPP

#include <dbrpp/elm/rec.hpp>

namespace dbr {

/**
 * @addtogroup Order
 * @{
 */

class OrderRef {
    DbrOrder* impl_;
public:
    explicit
    OrderRef(DbrOrder& impl) noexcept
        : impl_{&impl}
    {
    }
    operator DbrOrder&() const noexcept
    {
        return *impl_;
    }
    DbrOrder*
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
    DbrIden
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
    DbrJd
    settl_day() const noexcept
    {
        return impl_->i.settl_day;
    }
    StringRef
    ref() const noexcept
    {
        return StringRef{impl_->i.ref, MaxSize{DBR_REF_MAX}};
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
    DbrTicks
    ticks() const noexcept
    {
        return impl_->i.ticks;
    }
    DbrLots
    lots() const noexcept
    {
        return impl_->i.lots;
    }
    DbrLots
    resd() const noexcept
    {
        return impl_->i.resd;
    }
    DbrLots
    exec() const noexcept
    {
        return impl_->i.exec;
    }
    DbrTicks
    last_ticks() const noexcept
    {
        return impl_->i.last_ticks;
    }
    DbrLots
    last_lots() const noexcept
    {
        return impl_->i.last_lots;
    }
    DbrLots
    min_lots() const noexcept
    {
        return impl_->i.min_lots;
    }
    DbrMillis
    created() const noexcept
    {
        return impl_->created;
    }
    DbrMillis
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

} // dbr

#endif // DBRPP_ELM_ORDER_HPP
