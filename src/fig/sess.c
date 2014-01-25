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
#include <dbr/sess.h>

#include "trader.h"

#include <dbr/err.h>

DBR_API DbrTrader
dbr_sess_trader_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct FigTrader, sess_node_, node);
}

DBR_API DbrBool
dbr_sess_logon(struct DbrSess* sess, DbrTrader trader)
{
    if (trader->sess) {
        dbr_err_setf(DBR_EEXIST, "already logged-on '%.16s'", trader->rec->mnem);
        return DBR_FALSE;
    }
    trader->sess = sess;
    dbr_tree_insert(&sess->traders, trader->rec->id, &trader->sess_node_);
    return DBR_TRUE;
}

DBR_API void
dbr_sess_logoff(struct DbrSess* sess, DbrTrader trader)
{
    dbr_tree_remove(&sess->traders, &trader->sess_node_);
    trader->sess = NULL;
}
