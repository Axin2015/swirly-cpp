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
#ifndef DBRPP_ELM_TYPES_HPP
#define DBRPP_ELM_TYPES_HPP

#include <dbrpp/ash/string.hpp>

#include <dbr/elm/types.h>

namespace dbr {

/**
 * @addtogroup Simple
 * @{
 */

typedef MaxSize<DBR_DISPLAY_MAX> DisplayMax;
typedef MaxSize<DBR_EMAIL_MAX> EmailMax;
typedef MaxSize<DBR_MNEM_MAX> MnemMax;
typedef MaxSize<DBR_REF_MAX> RefMax;

const auto DISPLAY_MAX = DisplayMax();
const auto EMAIL_MAX = EmailMax();
const auto MNEM_MAX = MnemMax();
const auto REF_MAX = RefMax();

/** @} */

} // dbr

#endif // DBRPP_ELM_TYPES_HPP
