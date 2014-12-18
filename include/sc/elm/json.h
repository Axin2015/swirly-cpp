/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_JSON_H
#define SC_ASH_JSON_H

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
sc_json_accnt_len(const struct ScRec* rec);

SC_API char*
sc_json_write_accnt(char* buf, const struct ScRec* rec);

/** @} */

/**
 * @addtogroup ContrRec
 * @{
 */

SC_API size_t
sc_json_contr_len(const struct ScRec* rec);

SC_API char*
sc_json_write_contr(char* buf, const struct ScRec* rec);

/** @} */

/**
 * @addtogroup Perm
 * @{
 */

SC_API size_t
sc_json_perm_len(const struct ScPerm* perm);

SC_API char*
sc_json_write_perm(char* buf, const struct ScPerm* perm);

/** @} */

/**
 * @addtogroup Order
 * @{
 */

SC_API size_t
sc_json_order_len(const struct ScOrder* order);

SC_API char*
sc_json_write_order(char* buf, const struct ScOrder* order);

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

SC_API size_t
sc_json_exec_len(const struct ScExec* exec);

SC_API char*
sc_json_write_exec(char* buf, const struct ScExec* exec);

/** @} */

/**
 * @addtogroup Posn
 * @{
 */

SC_API size_t
sc_json_posn_len(const struct ScPosn* posn);

SC_API char*
sc_json_write_posn(char* buf, const struct ScPosn* posn);

/** @} */

/**
 * @addtogroup View
 * @{
 */

SC_API size_t
sc_json_view_len(const struct ScView* view);

SC_API char*
sc_json_write_view(char* buf, const struct ScView* view);

/** @} */

/**
 * @addtogroup Err
 * @{
 */

SC_API size_t
sc_json_err_len(int num, const char* msg);

SC_API char*
sc_json_write_err(char* buf, int num, const char* err);

/** @} */

#endif // SC_ASH_JSON_H
