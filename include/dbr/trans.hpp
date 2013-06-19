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
#ifndef DBR_TRANS_HPP
#define DBR_TRANS_HPP

#include <dbr/ctx.hpp>

namespace dbr {

class Trans {
    Ctx& ctx_;
    DbrTrans impl_;
public:
    ~Trans() noexcept
    {
        reset();
    }
    explicit
    Trans(Ctx& ctx) noexcept
    : ctx_(ctx)
    {
        impl_.first_match = nullptr;
    }

    // Copy semantics.

    Trans(const Trans&) = delete;

    Trans&
    operator =(const Trans&) = delete;

    void
    reset() noexcept
    {
        if (impl_.first_match) {
            ctx_.free_matches(impl_.first_match);
            impl_.first_match = nullptr;
        }
    }
    Order
    submit(DbrRec& trec, DbrRec& arec, const char* ref, DbrRec& mrec, int action,
           DbrTicks ticks, DbrLots lots, DbrLots min = 0, DbrFlags flags = 0)
    {
        reset();
        return ctx_.submit(trec, arec, ref, mrec, action, ticks, lots, min, flags, impl_);
    }
    size_t
    count() const noexcept
    {
        return impl_.count;
    }
    DbrLots
    taken() const noexcept
    {
        return impl_.taken;
    }
};

} // dbr

#endif // DBR_TRANS_HPP
