/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ELM_EXEC_HPP
#define SCPP_ELM_EXEC_HPP

#include <scpp/elm/rec.hpp>

namespace sc {

/**
 * @addtogroup Exec
 * @{
 */

class ExecRef {
    ScExec* impl_;
public:
    explicit
    ExecRef(ScExec& impl) noexcept
        : impl_{&impl}
    {
    }
    operator ScExec&() const noexcept
    {
        return *impl_;
    }
    ScExec*
    c_arg() const noexcept
    {
        return impl_;
    }
    bool
    operator ==(ExecRef rhs) const noexcept
    {
        return impl_->id == rhs.impl_->id;
    }
    bool
    operator !=(ExecRef rhs) const noexcept
    {
        return impl_->id != rhs.impl_->id;
    }
    ScIden
    id() const noexcept
    {
        return impl_->id;
    }
    ScIden
    order() const noexcept
    {
        return impl_->order;
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
    ScIden
    match() const noexcept
    {
        return impl_->match;
    }
    AccntRecRef
    cpty() const noexcept
    {
        return AccntRecRef{*impl_->cpty.rec};
    }
    int
    role() const noexcept
    {
        return impl_->role;
    }
    ScMillis
    created() const noexcept
    {
        return impl_->created;
    }
};

inline std::ostream&
operator <<(std::ostream& os, ExecRef exec)
{
    return os << "id=" << exec.id()
              << ",order=" << exec.order()
              << ",trader=" << exec.trec().mnem()
              << ",giveup=" << exec.grec().mnem()
              << ",contr=" << exec.crec().mnem()
              << ",settl_day=" << exec.settl_day()
              << ",ref=" << exec.ref()
              << ",state=" << exec.state()
              << ",action=" << exec.action()
              << ",ticks=" << exec.ticks()
              << ",lots=" << exec.lots()
              << ",resd=" << exec.resd()
              << ",exec=" << exec.exec()
              << ",last_ticks=" << exec.last_ticks()
              << ",last_lots=" << exec.last_lots()
              << ",min_lots=" << exec.min_lots()
              << ",match=" << exec.match()
              << ",cpty=" << exec.cpty().mnem()
              << ",role=" << exec.role()
              << ",created=" << exec.created();
}

/** @} */

} // sc

#endif // SCPP_ELM_EXEC_HPP
