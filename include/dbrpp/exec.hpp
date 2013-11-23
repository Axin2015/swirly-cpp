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
#ifndef DBRPP_EXEC_HPP
#define DBRPP_EXEC_HPP

#include <dbrpp/rec.hpp>

namespace dbr {

class ExecRef {
    DbrExec* impl_;
public:
    explicit
    ExecRef(DbrExec& impl) noexcept
        : impl_{&impl}
    {
    }
    operator DbrExec&() const noexcept
    {
        return *impl_;
    }
    DbrExec*
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
    DbrIden
    id() const noexcept
    {
        return impl_->id;
    }
    DbrIden
    order() const noexcept
    {
        return impl_->c.id;
    }
    int
    rev() const noexcept
    {
        return impl_->c.rev;
    }
    TraderRecRef
    trec() const noexcept
    {
        return TraderRecRef{*impl_->c.trader.rec};
    }
    AccntRecRef
    arec() const noexcept
    {
        return AccntRecRef{*impl_->c.accnt.rec};
    }
    ContrRecRef
    crec() const noexcept
    {
        return ContrRecRef{*impl_->c.contr.rec};
    }
    DbrDate
    settl_date() const noexcept
    {
        return impl_->c.settl_date;
    }
    Ref
    ref() const noexcept
    {
        return Ref{impl_->c.ref};
    }
    int
    action() const noexcept
    {
        return impl_->c.action;
    }
    DbrTicks
    ticks() const noexcept
    {
        return impl_->c.ticks;
    }
    DbrLots
    resd() const noexcept
    {
        return impl_->c.resd;
    }
    DbrLots
    exec() const noexcept
    {
        return impl_->c.exec;
    }
    DbrLots
    lots() const noexcept
    {
        return impl_->c.lots;
    }
    DbrIden
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
};

inline std::ostream&
operator <<(std::ostream& os, ExecRef exec)
{
    return os << "id=" << exec.id()
              << ",order=" << exec.order()
              << ",rev=" << exec.rev()
              << ",trec=" << exec.trec().mnem()
              << ",arec=" << exec.arec().mnem()
              << ",crec=" << exec.crec().mnem()
              << ",settl_date=" << exec.settl_date()
              << ",ref=" << exec.ref()
              << ",action=" << exec.action()
              << ",ticks=" << exec.ticks()
              << ",resd=" << exec.resd()
              << ",exec=" << exec.exec()
              << ",lots=" << exec.lots()
              << ",match=" << exec.match()
              << ",cpty=" << exec.cpty().mnem()
              << ",role=" << exec.role()
              << ",created=" << exec.created()
              << ",modified=" << exec.modified();
}

inline size_t
exec_len(const DbrExec& exec, DbrBool enriched) noexcept
{
    return dbr_exec_len(&exec, enriched);
}

inline char*
write_exec(char* buf, const DbrExec& exec, DbrBool enriched) noexcept
{
    return dbr_write_exec(buf, &exec, enriched);
}

inline const char*
read_exec(const char* buf, DbrExec& exec)
{
    buf = dbr_read_exec(buf, &exec);
    if (!buf)
        throw_exception();
    return buf;
}
} // dbr

#endif // DBRPP_EXEC_HPP
