/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#ifndef DBRPP_ELM_MEMB_HPP
#define DBRPP_ELM_MEMB_HPP

#include <dbrpp/elm/rec.hpp>

namespace dbr {

/**
 * @addtogroup Memb
 * @{
 */

class MembRef {
    DbrMemb* impl_;
public:
    explicit
    MembRef(DbrMemb& impl) noexcept
        : impl_{&impl}
    {
    }
    operator DbrMemb&() const noexcept
    {
        return *impl_;
    }
    DbrMemb*
    c_arg() noexcept
    {
        return impl_;
    }
    AccntRecRef
    urec() const noexcept
    {
        return AccntRecRef{*impl_->user.rec};
    }
    AccntRecRef
    grec() const noexcept
    {
        return AccntRecRef{*impl_->group.rec};
    }
};

inline std::ostream&
operator <<(std::ostream& os, MembRef memb)
{
    return os << "user=" << memb.urec().mnem()
              << ",group=" << memb.grec().mnem();
}

/** @} */

} // dbr

#endif // DBRPP_ELM_MEMB_HPP
