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
#ifndef DBRPP_MEMB_HPP
#define DBRPP_MEMB_HPP

#include <dbrpp/rec.hpp>

namespace dbr {

/**
 * @addtogroup TypesMemb
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
    user() const noexcept
    {
        return AccntRecRef{*impl_->user.rec};
    }
    AccntRecRef
    group() const noexcept
    {
        return AccntRecRef{*impl_->group.rec};
    }
};

inline std::ostream&
operator <<(std::ostream& os, MembRef memb)
{
    return os << "user=" << memb.user().mnem()
              << ",group=" << memb.group().mnem();
}

/** @} */

} // dbr

#endif // DBRPP_MEMB_HPP
