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
#ifndef DBRPP_JSON_HPP
#define DBRPP_JSON_HPP

#include <dbrpp/exec.hpp>
#include <dbrpp/memb.hpp>
#include <dbrpp/order.hpp>
#include <dbrpp/posn.hpp>
#include <dbrpp/rec.hpp>
#include <dbrpp/view.hpp>

#include <dbr/json.h>

#include <string>

namespace dbr {

/**
 * @addtogroup JsonAccnt
 * @{
 */

inline std::string
to_json(AccntRecRef ref)
{
    std::string s;
    s.resize(dbr_json_accnt_len(ref.c_arg()) + 1);
    dbr_json_write_accnt(&s[0], ref.c_arg());
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup JsonContr
 * @{
 */

inline std::string
to_json(ContrRecRef ref)
{
    std::string s;
    s.resize(dbr_json_contr_len(ref.c_arg()) + 1);
    dbr_json_write_contr(&s[0], ref.c_arg());
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup JsonMemb
 * @{
 */

inline std::string
to_json(MembRef ref)
{
    std::string s;
    s.resize(dbr_json_memb_len(ref.c_arg()) + 1);
    dbr_json_write_memb(&s[0], ref.c_arg());
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup JsonOrder
 * @{
 */

inline std::string
to_json(OrderRef ref)
{
    std::string s;
    s.resize(dbr_json_order_len(ref.c_arg()) + 1);
    dbr_json_write_order(&s[0], ref.c_arg());
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup JsonExec
 * @{
 */

inline std::string
to_json(ExecRef ref)
{
    std::string s;
    s.resize(dbr_json_exec_len(ref.c_arg()) + 1);
    dbr_json_write_exec(&s[0], ref.c_arg());
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup JsonPosn
 * @{
 */

inline std::string
to_json(PosnRef ref)
{
    std::string s;
    s.resize(dbr_json_posn_len(ref.c_arg()) + 1);
    dbr_json_write_posn(&s[0], ref.c_arg());
    s.resize(s.size() - 1);
    return s;
}

/** @} */

/**
 * @addtogroup JsonView
 * @{
 */

inline std::string
to_json(ViewRef ref)
{
    std::string s;
    s.resize(dbr_json_view_len(ref.c_arg()) + 1);
    dbr_json_write_view(&s[0], ref.c_arg());
    s.resize(s.size() - 1);
    return s;
}

/** @} */

} // dbr

#endif // DBRPP_JSON_HPP
