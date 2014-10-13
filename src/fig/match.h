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
#ifndef FIG_MATCH_H
#define FIG_MATCH_H

#include <dbr/elm/journ.h>
#include <dbr/elm/types.h>

struct DbrBank;
struct FigIndex;

// Used by matching engine to build a list of matches.

// Match taker with list of maker orders.

DBR_EXTERN DbrBool
fig_match_orders(struct DbrBook* book, struct DbrOrder* taker, struct DbrBank* bank,
                 DbrJourn journ, struct FigIndex* index, DbrPool pool, struct DbrTrans* trans);

#endif // FIG_MATCH_H
