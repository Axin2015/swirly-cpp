/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef ELM_PROTO_H
#define ELM_PROTO_H

#include <sc/elm/proto.h>

SC_EXTERN size_t
elm_proto_accnt_len(const struct ScRec* rec);

SC_EXTERN char*
elm_proto_write_accnt(char* buf, const struct ScRec* rec);

SC_EXTERN const char*
elm_proto_read_accnt(const char* buf, struct ScRec* rec);

SC_EXTERN size_t
elm_proto_contr_len(const struct ScRec* rec);

SC_EXTERN char*
elm_proto_write_contr(char* buf, const struct ScRec* rec);

SC_EXTERN const char*
elm_proto_read_contr(const char* buf, struct ScRec* rec);

SC_EXTERN size_t
elm_proto_perm_len(const struct ScPerm* perm, ScBool enriched);

SC_EXTERN char*
elm_proto_write_perm(char* buf, const struct ScPerm* perm, ScBool enriched);

SC_EXTERN const char*
elm_proto_read_perm(const char* buf, struct ScPerm* perm);

SC_EXTERN size_t
elm_proto_order_len(const struct ScOrder* order, ScBool enriched);

SC_EXTERN char*
elm_proto_write_order(char* buf, const struct ScOrder* order, ScBool enriched);

SC_EXTERN const char*
elm_proto_read_order(const char* buf, struct ScOrder* order);

SC_EXTERN size_t
elm_proto_exec_len(const struct ScExec* exec, ScBool enriched);

SC_EXTERN char*
elm_proto_write_exec(char* buf, const struct ScExec* exec, ScBool enriched);

SC_EXTERN const char*
elm_proto_read_exec(const char* buf, struct ScExec* exec);

SC_EXTERN size_t
elm_proto_posn_len(const struct ScPosn* posn, ScBool enriched);

SC_EXTERN char*
elm_proto_write_posn(char* buf, const struct ScPosn* posn, ScBool enriched);

SC_EXTERN const char*
elm_proto_read_posn(const char* buf, struct ScPosn* posn);

SC_EXTERN size_t
elm_proto_view_len(const struct ScView* view, ScBool enriched);

SC_EXTERN char*
elm_proto_write_view(char* buf, const struct ScView* view, ScBool enriched);

SC_EXTERN const char*
elm_proto_read_view(const char* buf, struct ScView* view);

#endif // ELM_PROTO_H
