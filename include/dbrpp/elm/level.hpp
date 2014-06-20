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
#ifndef DBRPP_ELM_LEVEL_HPP
#define DBRPP_ELM_LEVEL_HPP

#include <dbr/elm/types.h>

#include <iostream>

namespace dbr {

/**
 * @addtogroup TypesLevel
 * @{
 */

class LevelRef {
    DbrLevel* impl_;
public:
    explicit
    LevelRef(DbrLevel& impl) noexcept
        : impl_{&impl}
    {
    }
    operator DbrLevel&() const noexcept
    {
        return *impl_;
    }
    DbrLevel*
    c_arg() const noexcept
    {
        return impl_;
    }
    DbrTicks
    ticks() const noexcept
    {
        return impl_->ticks;
    }
    // Must be greater than zero.
    DbrLots
    lots() const noexcept
    {
        return impl_->lots;
    }
    size_t
    count() const noexcept
    {
        return impl_->count;
    }
};

inline std::ostream&
operator <<(std::ostream& os, LevelRef level)
{
    return os << "ticks=" << level.ticks()
              << ",lots=" << level.lots()
              << ",count=" << level.count();
}

/** @} */

} // dbr

#endif // DBRPP_ELM_LEVEL_HPP
