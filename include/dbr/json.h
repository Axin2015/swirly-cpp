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
#ifndef DBR_JSON_H
#define DBR_JSON_H

#include <dbr/defs.h>

#include <stddef.h> // size_t

struct DbrRec;
struct DbrMemb;
struct DbrOrder;
struct DbrExec;
struct DbrPosn;
struct DbrView;

/**
 * @addtogroup JsonAccnt
 * @{
 */

DBR_API size_t
dbr_json_accnt_len(const struct DbrRec* rec);

DBR_API char*
dbr_json_write_accnt(char* buf, const struct DbrRec* rec);

/** @} */

/**
 * @addtogroup JsonContr
 * @{
 */

DBR_API size_t
dbr_json_contr_len(const struct DbrRec* rec);

DBR_API char*
dbr_json_write_contr(char* buf, const struct DbrRec* rec);

/** @} */

/**
 * @addtogroup JsonMemb
 * @{
 */

DBR_API size_t
dbr_json_memb_len(const struct DbrMemb* memb);

DBR_API char*
dbr_json_write_memb(char* buf, const struct DbrMemb* memb);

/** @} */

/**
 * @addtogroup JsonOrder
 * @{
 */

DBR_API size_t
dbr_json_order_len(const struct DbrOrder* order);

DBR_API char*
dbr_json_write_order(char* buf, const struct DbrOrder* order);

/** @} */

/**
 * @addtogroup JsonExec
 * @{
 */

DBR_API size_t
dbr_json_exec_len(const struct DbrExec* exec);

DBR_API char*
dbr_json_write_exec(char* buf, const struct DbrExec* exec);

/** @} */

/**
 * @addtogroup JsonPosn
 * @{
 */

DBR_API size_t
dbr_json_posn_len(const struct DbrPosn* posn);

DBR_API char*
dbr_json_write_posn(char* buf, const struct DbrPosn* posn);

/** @} */

/**
 * @addtogroup JsonView
 * @{
 */

DBR_API size_t
dbr_json_view_len(const struct DbrView* view);

DBR_API char*
dbr_json_write_view(char* buf, const struct DbrView* view);

/** @} */

#endif // DBR_JSON_H
