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
#ifndef DBRPP_TYPES_HPP
#define DBRPP_TYPES_HPP

#include <dbrpp/string.hpp>

#include <dbr/types.h>

namespace dbr {

typedef NString<DBR_DISPLAY_MAX> Display;
typedef NString<DBR_EMAIL_MAX> Email;
typedef NString<DBR_MNEM_MAX> Mnem;
typedef NString<DBR_REF_MAX> Ref;

} // dbr

#endif // DBRPP_TYPES_HPP
