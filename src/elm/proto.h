/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
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
