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
#ifndef DBRPP_SESS_HPP
#define DBRPP_SESS_HPP

#include <dbrpp/order.hpp>
#include <dbrpp/posn.hpp>
#include <dbrpp/trade.hpp>

namespace dbr {

template <class DerivedT>
class IAccntSess : public DbrIAccntSess {
    static void
    trade(DbrAccntSess sess, DbrOrder* order, DbrTrade* trade, DbrPosn* posn) noexcept
    {
        static_cast<DerivedT*>(sess)->trade(Order(*order), Trade(*trade), Posn(*posn));
    }
    static const DbrAccntSessVtbl*
    vtbl() noexcept
    {
        static const DbrAccntSessVtbl VTBL = {
            trade
        };
        return &VTBL;
    }
public:
    IAccntSess()
        : DbrIAccntSess{ vtbl() }
    {
    }
};
} // dbr

#endif // DBRPP_SESS_HPP
