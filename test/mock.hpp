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
#ifndef MOCK_HPP
#define MOCK_HPP

#include <dbrpp/journ.hpp>
#include <dbrpp/model.hpp>

class Journ : public dbr::IJourn<Journ> {
public:
    void
    destroy() noexcept;

    DbrBool
    insert_exec_list(DbrSlNode* first, DbrBool enriched) noexcept;

    DbrBool
    insert_exec(DbrExec& exec, DbrBool enriched) noexcept;

    DbrBool
    update_exec(DbrIden id, DbrMillis modified) noexcept;
};

class Model : public dbr::IModel<Model> {
public:
    void
    destroy() noexcept;

    ssize_t
    read_entity(int type, DbrPool pool, DbrSlNode*& first) noexcept;
};

#endif // MOCK_HPP
