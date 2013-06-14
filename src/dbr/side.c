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
#include <dbr/side.h>

#include <elm/side.h>

DBR_API struct DbrDlNode*
dbr_side_first_order(DbrSide side)
{
    return elm_side_first_order(side);
}

DBR_API struct DbrDlNode*
dbr_side_last_order(DbrSide side)
{
    return elm_side_last_order(side);
}

DBR_API struct DbrDlNode*
dbr_side_end_order(DbrSide side)
{
    return elm_side_end_order(side);
}

DBR_API DbrBool
dbr_side_empty_order(DbrSide side)
{
    return elm_side_empty_order(side);
}

// SideLevel

DBR_API struct DbrRbNode*
dbr_side_find_level(DbrSide side, DbrTicks ticks)
{
    return elm_side_find_level(side, ticks);
}

DBR_API struct DbrRbNode*
dbr_side_first_level(DbrSide side)
{
    return elm_side_first_level(side);
}

DBR_API struct DbrRbNode*
dbr_side_last_level(DbrSide side)
{
    return elm_side_last_level(side);
}

DBR_API struct DbrRbNode*
dbr_side_end_level(DbrSide side)
{
    return elm_side_end_level(side);
}

DBR_API DbrBool
dbr_side_empty_level(DbrSide side)
{
    return elm_side_empty_level(side);
}

// SideLast

DBR_API DbrTicks
dbr_side_last_ticks(DbrSide side)
{
    return elm_side_last_ticks(side);
}

DBR_API DbrLots
dbr_side_last_lots(DbrSide side)
{
    return elm_side_last_lots(side);
}

DBR_API DbrMillis
dbr_side_last_time(DbrSide side)
{
    return elm_side_last_time(side);
}
