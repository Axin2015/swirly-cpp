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
#ifndef ELM_FORMAT_H
#define ELM_FORMAT_H

#include <dbr/elm/format.h>

#include <stddef.h> // size_t

DBR_EXTERN const char*
elm_action_str(int action);

DBR_EXTERN size_t
elm_action_len(int action);

DBR_EXTERN const char*
elm_role_str(int role);

DBR_EXTERN size_t
elm_role_len(int role);

DBR_EXTERN const char*
elm_state_str(int state);

DBR_EXTERN size_t
elm_state_len(int state);

#endif // ELM_FORMAT_H
