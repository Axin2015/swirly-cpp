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
#ifndef ELM_PROTO_H
#define ELM_PROTO_H

#include <dbr/elm/proto.h>

DBR_EXTERN size_t
elm_proto_accnt_len(const struct DbrRec* rec);

DBR_EXTERN char*
elm_proto_write_accnt(char* buf, const struct DbrRec* rec);

DBR_EXTERN const char*
elm_proto_read_accnt(const char* buf, struct DbrRec* rec);

DBR_EXTERN size_t
elm_proto_contr_len(const struct DbrRec* rec);

DBR_EXTERN char*
elm_proto_write_contr(char* buf, const struct DbrRec* rec);

DBR_EXTERN const char*
elm_proto_read_contr(const char* buf, struct DbrRec* rec);

DBR_EXTERN size_t
elm_proto_perm_len(const struct DbrPerm* perm, DbrBool enriched);

DBR_EXTERN char*
elm_proto_write_perm(char* buf, const struct DbrPerm* perm, DbrBool enriched);

DBR_EXTERN const char*
elm_proto_read_perm(const char* buf, struct DbrPerm* perm);

DBR_EXTERN size_t
elm_proto_order_len(const struct DbrOrder* order, DbrBool enriched);

DBR_EXTERN char*
elm_proto_write_order(char* buf, const struct DbrOrder* order, DbrBool enriched);

DBR_EXTERN const char*
elm_proto_read_order(const char* buf, struct DbrOrder* order);

DBR_EXTERN size_t
elm_proto_exec_len(const struct DbrExec* exec, DbrBool enriched);

DBR_EXTERN char*
elm_proto_write_exec(char* buf, const struct DbrExec* exec, DbrBool enriched);

DBR_EXTERN const char*
elm_proto_read_exec(const char* buf, struct DbrExec* exec);

DBR_EXTERN size_t
elm_proto_posn_len(const struct DbrPosn* posn, DbrBool enriched);

DBR_EXTERN char*
elm_proto_write_posn(char* buf, const struct DbrPosn* posn, DbrBool enriched);

DBR_EXTERN const char*
elm_proto_read_posn(const char* buf, struct DbrPosn* posn);

DBR_EXTERN size_t
elm_proto_view_len(const struct DbrView* view, DbrBool enriched);

DBR_EXTERN char*
elm_proto_write_view(char* buf, const struct DbrView* view, DbrBool enriched);

DBR_EXTERN const char*
elm_proto_read_view(const char* buf, struct DbrView* view);

#endif // ELM_PROTO_H
