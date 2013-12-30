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
struct DbrExec;
struct DbrPosn;
struct DbrView;

/**
 * @addtogroup ProtoRec
 * @{
 */

/**
 * Returns -1 if type is unrecognised.
 */

DBR_API size_t
dbr_rec_len(const struct DbrRec* rec);

DBR_API char*
dbr_write_rec(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_read_rec(const char* buf, struct DbrRec* rec);

/** @} */

/**
 * @addtogroup ProtoTrader
 * @{
 */

DBR_API size_t
dbr_trader_len(const struct DbrRec* rec);

DBR_API char*
dbr_write_trader(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_read_trader(const char* buf, struct DbrRec* rec);

/** @} */

/**
 * @addtogroup ProtoAccnt
 * @{
 */

DBR_API size_t
dbr_accnt_len(const struct DbrRec* rec);

DBR_API char*
dbr_write_accnt(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_read_accnt(const char* buf, struct DbrRec* rec);

/** @} */

/**
 * @addtogroup ProtoContr
 * @{
 */

DBR_API size_t
dbr_contr_len(const struct DbrRec* rec);

DBR_API char*
dbr_write_contr(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_read_contr(const char* buf, struct DbrRec* rec);

/** @} */

/**
 * @addtogroup ProtoOrder
 * @{
 */

DBR_API size_t
dbr_order_len(const struct DbrOrder* order, DbrBool enriched);

DBR_API char*
dbr_write_order(char* buf, const struct DbrOrder* order, DbrBool enriched);

DBR_API const char*
dbr_read_order(const char* buf, struct DbrOrder* order);

/** @} */

/**
 * @addtogroup ProtoExec
 * @{
 */

DBR_API size_t
dbr_exec_len(const struct DbrExec* exec, DbrBool enriched);

DBR_API char*
dbr_write_exec(char* buf, const struct DbrExec* exec, DbrBool enriched);

DBR_API const char*
dbr_read_exec(const char* buf, struct DbrExec* exec);

/** @} */

/**
 * @addtogroup ProtoMemb
 * @{
 */

DBR_API size_t
dbr_memb_len(const struct DbrMemb* memb, DbrBool enriched);

DBR_API char*
dbr_write_memb(char* buf, const struct DbrMemb* memb, DbrBool enriched);

DBR_API const char*
dbr_read_memb(const char* buf, struct DbrMemb* memb);

/** @} */

/**
 * @addtogroup ProtoPosn
 * @{
 */

DBR_API size_t
dbr_posn_len(const struct DbrPosn* posn, DbrBool enriched);

DBR_API char*
dbr_write_posn(char* buf, const struct DbrPosn* posn, DbrBool enriched);

DBR_API const char*
dbr_read_posn(const char* buf, struct DbrPosn* posn);

/** @} */

/**
 * @addtogroup ProtoView
 * @{
 */

DBR_API size_t
dbr_view_len(const struct DbrView* view, DbrBool enriched);

DBR_API char*
dbr_write_view(char* buf, const struct DbrView* view, DbrBool enriched);

DBR_API const char*
dbr_read_view(const char* buf, struct DbrView* view);

/** @} */

#endif // DBR_PROTO_H
