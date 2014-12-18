/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_JSON_H
#define DBR_ASH_JSON_H

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
dbr_json_accnt_len(const struct DbrRec* rec);

DBR_API char*
dbr_json_write_accnt(char* buf, const struct DbrRec* rec);

/** @} */

/**
 * @addtogroup ContrRec
 * @{
 */

DBR_API size_t
dbr_json_contr_len(const struct DbrRec* rec);

DBR_API char*
dbr_json_write_contr(char* buf, const struct DbrRec* rec);

/** @} */

/**
 * @addtogroup Perm
 * @{
 */

DBR_API size_t
dbr_json_perm_len(const struct DbrPerm* perm);

DBR_API char*
dbr_json_write_perm(char* buf, const struct DbrPerm* perm);

/** @} */

/**
 * @addtogroup Order
 * @{
 */

DBR_API size_t
dbr_json_order_len(const struct DbrOrder* order);

DBR_API char*
dbr_json_write_order(char* buf, const struct DbrOrder* order);

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

DBR_API size_t
dbr_json_exec_len(const struct DbrExec* exec);

DBR_API char*
dbr_json_write_exec(char* buf, const struct DbrExec* exec);

/** @} */

/**
 * @addtogroup Posn
 * @{
 */

DBR_API size_t
dbr_json_posn_len(const struct DbrPosn* posn);

DBR_API char*
dbr_json_write_posn(char* buf, const struct DbrPosn* posn);

/** @} */

/**
 * @addtogroup View
 * @{
 */

DBR_API size_t
dbr_json_view_len(const struct DbrView* view);

DBR_API char*
dbr_json_write_view(char* buf, const struct DbrView* view);

/** @} */

/**
 * @addtogroup Err
 * @{
 */

DBR_API size_t
dbr_json_err_len(int num, const char* msg);

DBR_API char*
dbr_json_write_err(char* buf, int num, const char* err);

/** @} */

#endif // DBR_ASH_JSON_H
