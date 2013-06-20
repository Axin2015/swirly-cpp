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
#ifndef DBR_ACCNT_HPP
#define DBR_ACCNT_HPP

#include <dbr/accnt.h>

#include <iostream>

namespace dbr {

class Accnt {
    DbrAccnt impl_;
public:
    Accnt(DbrAccnt impl) noexcept
        : impl_(impl)
    {
    }
    bool
    operator ==(const Accnt& rhs) const noexcept
    {
        return id() == rhs.id();
    }
    bool
    operator !=(const Accnt& rhs) const noexcept
    {
        return id() != rhs.id();
    }
    DbrIden
    id() const noexcept
    {
        return dbr_accnt_id(impl_);
    }
};

inline std::ostream&
operator <<(std::ostream& os, Accnt accnt)
{
    return os << "id=" << accnt.id();
}

} // dbr

#endif // DBR_ACCNT_HPP
