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
#ifndef DBR_PROTO_H
#define DBR_PROTO_H

#include <dbr/defs.h>

#include <stddef.h> // size_t

struct DbrRec;
struct DbrOrder;
struct DbrMemb;
struct DbrTrade;
struct DbrPosn;

/**
 * @addtogroup Proto
 * @{
 */

DBR_API size_t
dbr_trader_len(const struct DbrRec* rec);

DBR_API char*
dbr_write_trader(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_read_trader(const char* buf, struct DbrRec* rec);

DBR_API size_t
dbr_accnt_len(const struct DbrRec* rec);

DBR_API char*
dbr_write_accnt(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_read_accnt(const char* buf, struct DbrRec* rec);

DBR_API size_t
dbr_contr_len(const struct DbrRec* rec);

DBR_API char*
dbr_write_contr(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_read_contr(const char* buf, struct DbrRec* rec);

/**
 * @brief Returns -1 if type is unrecognised.
 */

DBR_API size_t
dbr_rec_len(const struct DbrRec* rec);

DBR_API char*
dbr_write_rec(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_read_rec(const char* buf, struct DbrRec* rec);

DBR_API size_t
dbr_order_len(const struct DbrOrder* order);

DBR_API char*
dbr_write_order(char* buf, const struct DbrOrder* order);

DBR_API const char*
dbr_read_order(const char* buf, struct DbrOrder* order);

DBR_API size_t
dbr_memb_len(const struct DbrMemb* memb);

DBR_API char*
dbr_write_memb(char* buf, const struct DbrMemb* memb);

DBR_API const char*
dbr_read_memb(const char* buf, struct DbrMemb* memb);

DBR_API size_t
dbr_trade_len(const struct DbrTrade* trade);

DBR_API char*
dbr_write_trade(char* buf, const struct DbrTrade* trade);

DBR_API const char*
dbr_read_trade(const char* buf, struct DbrTrade* trade);

DBR_API size_t
dbr_posn_len(const struct DbrPosn* posn);

DBR_API char*
dbr_write_posn(char* buf, const struct DbrPosn* posn);

DBR_API const char*
dbr_read_posn(const char* buf, struct DbrPosn* posn);

/** @} */

#endif // DBR_PROTO_H
