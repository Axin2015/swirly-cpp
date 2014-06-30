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
#ifndef DBR_ASH_PROTO_H
#define DBR_ASH_PROTO_H

#include <dbr/ash/defs.h>

#include <stddef.h> // size_t

struct DbrRec;
struct DbrPerm;
struct DbrOrder;
struct DbrExec;
struct DbrPosn;
struct DbrView;

/**
 * @addtogroup AccntRec
 * @{
 */

DBR_API size_t
dbr_proto_accnt_len(const struct DbrRec* rec);

DBR_API char*
dbr_proto_write_accnt(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_proto_read_accnt(const char* buf, struct DbrRec* rec);

/** @} */

/**
 * @addtogroup ContrRec
 * @{
 */

DBR_API size_t
dbr_proto_contr_len(const struct DbrRec* rec);

DBR_API char*
dbr_proto_write_contr(char* buf, const struct DbrRec* rec);

DBR_API const char*
dbr_proto_read_contr(const char* buf, struct DbrRec* rec);

/** @} */

/**
 * @addtogroup Perm
 * @{
 */

DBR_API size_t
dbr_proto_perm_len(const struct DbrPerm* perm, DbrBool enriched);

DBR_API char*
dbr_proto_write_perm(char* buf, const struct DbrPerm* perm, DbrBool enriched);

DBR_API const char*
dbr_proto_read_perm(const char* buf, struct DbrPerm* perm);

/** @} */

/**
 * @addtogroup Order
 * @{
 */

DBR_API size_t
dbr_proto_order_len(const struct DbrOrder* order, DbrBool enriched);

DBR_API char*
dbr_proto_write_order(char* buf, const struct DbrOrder* order, DbrBool enriched);

DBR_API const char*
dbr_proto_read_order(const char* buf, struct DbrOrder* order);

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

DBR_API size_t
dbr_proto_exec_len(const struct DbrExec* exec, DbrBool enriched);

DBR_API char*
dbr_proto_write_exec(char* buf, const struct DbrExec* exec, DbrBool enriched);

DBR_API const char*
dbr_proto_read_exec(const char* buf, struct DbrExec* exec);

/** @} */

/**
 * @addtogroup Posn
 * @{
 */

DBR_API size_t
dbr_proto_posn_len(const struct DbrPosn* posn, DbrBool enriched);

DBR_API char*
dbr_proto_write_posn(char* buf, const struct DbrPosn* posn, DbrBool enriched);

DBR_API const char*
dbr_proto_read_posn(const char* buf, struct DbrPosn* posn);

/** @} */

/**
 * @addtogroup View
 * @{
 */

DBR_API size_t
dbr_proto_view_len(const struct DbrView* view, DbrBool enriched);

DBR_API char*
dbr_proto_write_view(char* buf, const struct DbrView* view, DbrBool enriched);

DBR_API const char*
dbr_proto_read_view(const char* buf, struct DbrView* view);

/** @} */

#endif // DBR_ASH_PROTO_H
