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
#ifndef DBRPP_POSN_HPP
#define DBRPP_POSN_HPP

#include <dbrpp/rec.hpp>

namespace dbr {

class Posn {
    DbrPosn& impl_;
public:
    explicit
    Posn(DbrPosn& impl) noexcept
        : impl_(impl)
    {
    }
    operator DbrPosn&() const noexcept
    {
        return impl_;
    }
    DbrPosn*
    c_arg() noexcept
    {
        return &impl_;
    }
    bool
    operator ==(Posn rhs) const noexcept
    {
        return impl_.id == rhs.impl_.id;
    }
    bool
    operator !=(Posn rhs) const noexcept
    {
        return impl_.id != rhs.impl_.id;
    }
    DbrIden
    id() const noexcept
    {
        return impl_.id;
    }
    AccntRec
    arec() const noexcept
    {
        return AccntRec(*impl_.accnt.rec);
    }
    InstrRec
    irec() const noexcept
    {
        return InstrRec(*impl_.instr.rec);
    }
    DbrDate
    settl_date() const noexcept
    {
        return impl_.settl_date;
    }
    DbrLicks
    buy_licks() const noexcept
    {
        return impl_.buy_licks;
    }
    DbrLots
    buy_lots() const noexcept
    {
        return impl_.buy_lots;
    }
    DbrLicks
    sell_licks() const noexcept
    {
        return impl_.sell_licks;
    }
    DbrLots
    sell_lots() const noexcept
    {
        return impl_.sell_lots;
    }
};

inline std::ostream&
operator <<(std::ostream& os, Posn posn)
{
    return os << "id=" << posn.id()
              << ",arec=" << posn.arec().mnem()
              << ",irec=" << posn.irec().mnem()
              << ",settl_date=" << posn.settl_date()
              << ",buy_licks=" << posn.buy_licks()
              << ",buy_lots=" << posn.buy_lots()
              << ",sell_licks=" << posn.sell_licks()
              << ",sell_lots=" << posn.sell_lots();
}

inline size_t
entity_len(Posn posn) noexcept
{
    return dbr_posn_len(posn.c_arg());
}

inline char*
write_entity(char* buf, Posn posn) noexcept
{
    return dbr_write_posn(buf, posn.c_arg());
}

inline const char*
read_entity(const char* buf, Posn posn)
{
    buf = dbr_read_posn(buf, posn.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}
} // dbr

#endif // DBRPP_POSN_HPP
