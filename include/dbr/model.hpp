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
#ifndef DBR_MODEL_HPP
#define DBR_MODEL_HPP

#include <dbr/except.hpp>

#include <dbr/model.h>

namespace dbr {

template <class DerivedT>
class IModel : public DbrIModel {
    static ssize_t
    read_entity(DbrModel model, int type, DbrSlNode** first) noexcept
    {
        return static_cast<DerivedT*>(model)->read_entity(type, *first);
    }
    static DbrSlNode*
    end_entity(DbrModel model) noexcept
    {
        return static_cast<DerivedT*>(model)->end_entity();
    }
    static const DbrModelVtbl*
    vtbl() noexcept
    {
        static const DbrModelVtbl VTBL = {
            read_entity,
            end_entity
        };
        return &VTBL;
    }
public:
    IModel()
        : DbrIModel{ vtbl() }
    {
    }
};

inline size_t
read_entity(DbrModel model, int type, DbrSlNode*& first)
{
    const auto size = model->vtbl->read_entity(model, type, &first);
    if (size < 0)
        throw_exception();
    return size;
}

inline DbrSlNode*
end_entity(DbrModel model) noexcept
{
    return model->vtbl->end_entity(model);
}

} // dbr

#endif // DBR_MODEL_HPP
