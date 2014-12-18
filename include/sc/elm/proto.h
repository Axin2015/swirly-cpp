/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_PROTO_H
#define SC_ASH_PROTO_H

#include <sc/ash/defs.h>

#include <stddef.h> // size_t

struct ScRec;
struct ScPerm;
struct ScOrder;
struct ScExec;
struct ScPosn;
struct ScView;

/**
 * @addtogroup AccntRec
 * @{
 */

SC_API size_t
sc_proto_accnt_len(const struct ScRec* rec);

SC_API char*
sc_proto_write_accnt(char* buf, const struct ScRec* rec);

SC_API const char*
sc_proto_read_accnt(const char* buf, struct ScRec* rec);

/** @} */

/**
 * @addtogroup ContrRec
 * @{
 */

SC_API size_t
sc_proto_contr_len(const struct ScRec* rec);

SC_API char*
sc_proto_write_contr(char* buf, const struct ScRec* rec);

SC_API const char*
sc_proto_read_contr(const char* buf, struct ScRec* rec);

/** @} */

/**
 * @addtogroup Perm
 * @{
 */

SC_API size_t
sc_proto_perm_len(const struct ScPerm* perm, ScBool enriched);

SC_API char*
sc_proto_write_perm(char* buf, const struct ScPerm* perm, ScBool enriched);

SC_API const char*
sc_proto_read_perm(const char* buf, struct ScPerm* perm);

/** @} */

/**
 * @addtogroup Order
 * @{
 */

SC_API size_t
sc_proto_order_len(const struct ScOrder* order, ScBool enriched);

SC_API char*
sc_proto_write_order(char* buf, const struct ScOrder* order, ScBool enriched);

SC_API const char*
sc_proto_read_order(const char* buf, struct ScOrder* order);

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

SC_API size_t
sc_proto_exec_len(const struct ScExec* exec, ScBool enriched);

SC_API char*
sc_proto_write_exec(char* buf, const struct ScExec* exec, ScBool enriched);

SC_API const char*
sc_proto_read_exec(const char* buf, struct ScExec* exec);

/** @} */

/**
 * @addtogroup Posn
 * @{
 */

SC_API size_t
sc_proto_posn_len(const struct ScPosn* posn, ScBool enriched);

SC_API char*
sc_proto_write_posn(char* buf, const struct ScPosn* posn, ScBool enriched);

SC_API const char*
sc_proto_read_posn(const char* buf, struct ScPosn* posn);

/** @} */

/**
 * @addtogroup View
 * @{
 */

SC_API size_t
sc_proto_view_len(const struct ScView* view, ScBool enriched);

SC_API char*
sc_proto_write_view(char* buf, const struct ScView* view, ScBool enriched);

SC_API const char*
sc_proto_read_view(const char* buf, struct ScView* view);

/** @} */

#endif // SC_ASH_PROTO_H
