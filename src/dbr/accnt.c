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
#include <dbr/accnt.h>

#include <elm/accnt.h>

DBR_API DbrIden
dbr_accnt_id(DbrAccnt accnt)
{
    return elm_accnt_id(accnt);
}

// AccntTrade

DBR_API struct DbrRbNode*
dbr_accnt_find_trade_id(DbrAccnt accnt, DbrIden id)
{
    return elm_accnt_find_trade_id(accnt, id);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_trade(DbrAccnt accnt)
{
    return elm_accnt_first_trade(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_trade(DbrAccnt accnt)
{
    return elm_accnt_last_trade(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_end_trade(DbrAccnt accnt)
{
    return elm_accnt_end_trade(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_trade(DbrAccnt accnt)
{
    return elm_accnt_empty_trade(accnt);
}

// AccntPosn.

DBR_API struct DbrRbNode*
dbr_accnt_find_posn_id(DbrAccnt accnt, DbrIden id)
{
    return elm_accnt_find_posn_id(accnt, id);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_posn(DbrAccnt accnt)
{
    return elm_accnt_first_posn(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_posn(DbrAccnt accnt)
{
    return elm_accnt_last_posn(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_end_posn(DbrAccnt accnt)
{
    return elm_accnt_end_posn(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_posn(DbrAccnt accnt)
{
    return elm_accnt_empty_posn(accnt);
}

DBR_API void
dbr_accnt_set_sess(DbrAccnt accnt, DbrAccntSess sess)
{
    elm_accnt_set_sess(accnt, sess);
}

DBR_API DbrAccntSess
dbr_accnt_sess(DbrAccnt accnt)
{
    return elm_accnt_sess(accnt);
}
