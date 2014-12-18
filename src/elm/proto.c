/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "proto.h"

#include <dbr/elm/conv.h>
#include <dbr/elm/types.h>

#include <dbr/ash/err.h>
#include <dbr/ash/pack.h>

#include <stdlib.h> // abort()

static const char ACCNT_FORMAT[] = "lmss";
static const char CONTR_FORMAT[] = "lmsmmmiiiiill";
static const char PERM_FORMAT[] = "ll";
static const char ORDER_FORMAT[] = "llllisiilllllllll";
static const char EXEC_FORMAT[] = "lllllisiillllllllill";
static const char POSN_FORMAT[] = "lllllll";
static const char VIEW_FORMAT[] = "lillzllzllzllzllzllzl";

DBR_EXTERN size_t
elm_proto_accnt_len(const struct DbrRec* rec)
{
    return dbr_pack_lenf(ACCNT_FORMAT,
                         rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                         DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_EXTERN char*
elm_proto_write_accnt(char* buf, const struct DbrRec* rec)
{
    return dbr_packf(buf, ACCNT_FORMAT,
                     rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                     DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_EXTERN const char*
elm_proto_read_accnt(const char* buf, struct DbrRec* rec)
{
    rec->type = DBR_ENTITY_ACCNT;
    rec->accnt.state = NULL;
    return dbr_unpackf(buf, ACCNT_FORMAT,
                       &rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                       DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_EXTERN size_t
elm_proto_contr_len(const struct DbrRec* rec)
{
    return dbr_pack_lenf(CONTR_FORMAT,
                         rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                         rec->contr.asset_type, rec->contr.asset, rec->contr.ccy,
                         rec->contr.tick_numer, rec->contr.tick_denom, rec->contr.lot_numer,
                         rec->contr.lot_denom, rec->contr.pip_dp, rec->contr.min_lots,
                         rec->contr.max_lots);
}

DBR_EXTERN char*
elm_proto_write_contr(char* buf, const struct DbrRec* rec)
{
    return dbr_packf(buf, CONTR_FORMAT,
                     rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                     rec->contr.asset_type, rec->contr.asset, rec->contr.ccy,
                     rec->contr.tick_numer, rec->contr.tick_denom, rec->contr.lot_numer,
                     rec->contr.lot_denom, rec->contr.pip_dp, rec->contr.min_lots,
                     rec->contr.max_lots);
}

DBR_EXTERN const char*
elm_proto_read_contr(const char* buf, struct DbrRec* rec)
{
    rec->type = DBR_ENTITY_CONTR;
    const char* end = dbr_unpackf(buf, CONTR_FORMAT,
                                  &rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                                  rec->contr.asset_type, rec->contr.asset, rec->contr.ccy,
                                  &rec->contr.tick_numer, &rec->contr.tick_denom,
                                  &rec->contr.lot_numer, &rec->contr.lot_denom,
                                  &rec->contr.pip_dp, &rec->contr.min_lots, &rec->contr.max_lots);

    // Derive transient fields.
    rec->contr.price_inc = dbr_fract_to_real(rec->contr.tick_numer, rec->contr.tick_denom);
    rec->contr.qty_inc = dbr_fract_to_real(rec->contr.lot_numer, rec->contr.lot_denom);
    rec->contr.price_dp = dbr_real_to_dp(rec->contr.price_inc);
    rec->contr.qty_dp = dbr_real_to_dp(rec->contr.qty_inc);
    return end;
}

DBR_EXTERN size_t
elm_proto_perm_len(const struct DbrPerm* perm, DbrBool enriched)
{
    size_t n;
    if (enriched) {
        n = dbr_pack_lenf(PERM_FORMAT,
                          perm->giveup.rec->id, perm->trader.rec->id);
    } else {
        n = dbr_pack_lenf(PERM_FORMAT,
                          perm->giveup.id_only, perm->trader.id_only);
    }
    return n;
}

DBR_EXTERN char*
elm_proto_write_perm(char* buf, const struct DbrPerm* perm, DbrBool enriched)
{
    if (enriched) {
        buf = dbr_packf(buf, PERM_FORMAT,
                        perm->giveup.rec->id, perm->trader.rec->id);
    } else {
        buf = dbr_packf(buf, PERM_FORMAT,
                        perm->giveup.id_only, perm->trader.id_only);
    }
    return buf;
}

DBR_EXTERN const char*
elm_proto_read_perm(const char* buf, struct DbrPerm* perm)
{
    return dbr_unpackf(buf, PERM_FORMAT, &perm->giveup.id_only, &perm->trader.id_only);
}

DBR_EXTERN size_t
elm_proto_order_len(const struct DbrOrder* order, DbrBool enriched)
{
    size_t n;
    if (enriched) {
        n = dbr_pack_lenf(ORDER_FORMAT,
                          order->id, order->i.trader.rec->id, order->i.giveup.rec->id,
                          order->i.contr.rec->id, order->i.settl_day, DBR_REF_MAX, order->i.ref,
                          order->i.state, order->i.action, order->i.ticks, order->i.lots,
                          order->i.resd, order->i.exec, order->i.last_ticks, order->i.last_lots,
                          order->i.min_lots, order->created, order->modified);
    } else {
        n = dbr_pack_lenf(ORDER_FORMAT,
                          order->id, order->i.trader.id_only, order->i.giveup.id_only,
                          order->i.contr.id_only, order->i.settl_day, DBR_REF_MAX, order->i.ref,
                          order->i.state, order->i.action, order->i.ticks, order->i.lots,
                          order->i.resd, order->i.exec, order->i.last_ticks, order->i.last_lots,
                          order->i.min_lots, order->created, order->modified);
    }
    return n;
}

DBR_EXTERN char*
elm_proto_write_order(char* buf, const struct DbrOrder* order, DbrBool enriched)
{
    if (enriched) {
        buf = dbr_packf(buf, ORDER_FORMAT,
                        order->id, order->i.trader.rec->id, order->i.giveup.rec->id,
                        order->i.contr.rec->id, order->i.settl_day, DBR_REF_MAX,
                        order->i.ref, order->i.state, order->i.action, order->i.ticks,
                        order->i.lots, order->i.resd, order->i.exec, order->i.last_ticks,
                        order->i.last_lots, order->i.min_lots, order->created, order->modified);
    } else {
        buf = dbr_packf(buf, ORDER_FORMAT,
                        order->id, order->i.trader.id_only, order->i.giveup.id_only,
                        order->i.contr.id_only, order->i.settl_day, DBR_REF_MAX,
                        order->i.ref, order->i.state, order->i.action, order->i.ticks,
                        order->i.lots, order->i.resd, order->i.exec, order->i.last_ticks,
                        order->i.last_lots, order->i.min_lots, order->created, order->modified);
    }
    return buf;
}

DBR_EXTERN const char*
elm_proto_read_order(const char* buf, struct DbrOrder* order)
{
    return dbr_unpackf(buf, ORDER_FORMAT,
                       &order->id, &order->i.trader.id_only, &order->i.giveup.id_only,
                       &order->i.contr.id_only, &order->i.settl_day, DBR_REF_MAX,
                       order->i.ref, &order->i.state, &order->i.action, &order->i.ticks,
                       &order->i.lots, &order->i.resd, &order->i.exec, &order->i.last_ticks,
                       &order->i.last_lots, &order->i.min_lots, &order->created, &order->modified);
}

DBR_EXTERN size_t
elm_proto_exec_len(const struct DbrExec* exec, DbrBool enriched)
{
    size_t n;
    if (enriched) {
        const DbrIden cpty = exec->cpty.rec ? exec->cpty.rec->id : 0;
        n = dbr_pack_lenf(EXEC_FORMAT,
                          exec->id, exec->order, exec->i.trader.rec->id, exec->i.giveup.rec->id,
                          exec->i.contr.rec->id, exec->i.settl_day, DBR_REF_MAX, exec->i.ref,
                          exec->i.state, exec->i.action, exec->i.ticks, exec->i.lots,
                          exec->i.resd, exec->i.exec, exec->i.last_ticks, exec->i.last_lots,
                          exec->i.min_lots, exec->match, exec->role, cpty, exec->created);
    } else {
        n = dbr_pack_lenf(EXEC_FORMAT,
                          exec->id, exec->order, exec->i.trader.id_only, exec->i.giveup.id_only,
                          exec->i.contr.id_only, exec->i.settl_day, DBR_REF_MAX, exec->i.ref,
                          exec->i.state, exec->i.action, exec->i.ticks, exec->i.lots,
                          exec->i.resd, exec->i.exec, exec->i.last_ticks, exec->i.last_lots,
                          exec->i.min_lots, exec->match, exec->role, exec->cpty.id_only,
                          exec->created);
    }
    return n;
}

DBR_EXTERN char*
elm_proto_write_exec(char* buf, const struct DbrExec* exec, DbrBool enriched)
{
    if (enriched) {
        const DbrIden cpty = exec->cpty.rec ? exec->cpty.rec->id : 0;
        buf = dbr_packf(buf, EXEC_FORMAT,
                        exec->id, exec->order, exec->i.trader.rec->id, exec->i.giveup.rec->id,
                        exec->i.contr.rec->id, exec->i.settl_day, DBR_REF_MAX, exec->i.ref,
                        exec->i.state, exec->i.action, exec->i.ticks, exec->i.lots,
                        exec->i.resd, exec->i.exec, exec->i.last_ticks, exec->i.last_lots,
                        exec->i.min_lots, exec->match, exec->role, cpty, exec->created);
    } else {
        buf = dbr_packf(buf, EXEC_FORMAT,
                        exec->id, exec->order, exec->i.trader.id_only, exec->i.giveup.id_only,
                        exec->i.contr.id_only, exec->i.settl_day, DBR_REF_MAX, exec->i.ref,
                        exec->i.state, exec->i.action, exec->i.ticks, exec->i.lots,
                        exec->i.resd, exec->i.exec, exec->i.last_ticks, exec->i.last_lots,
                        exec->i.min_lots, exec->match, exec->role, exec->cpty.id_only,
                        exec->created);
    }
    return buf;
}

DBR_EXTERN const char*
elm_proto_read_exec(const char* buf, struct DbrExec* exec)
{
    return dbr_unpackf(buf, EXEC_FORMAT,
                       &exec->id, &exec->order, &exec->i.trader.id_only, &exec->i.giveup.id_only,
                       &exec->i.contr.id_only, &exec->i.settl_day, DBR_REF_MAX, exec->i.ref,
                       &exec->i.state, &exec->i.action, &exec->i.ticks, &exec->i.lots,
                       &exec->i.resd, &exec->i.exec, &exec->i.last_ticks, &exec->i.last_lots,
                       &exec->i.min_lots, &exec->match, &exec->role, &exec->cpty.id_only,
                       &exec->created);
}

DBR_EXTERN size_t
elm_proto_posn_len(const struct DbrPosn* posn, DbrBool enriched)
{
    size_t n;
    if (enriched) {
        n = dbr_pack_lenf(POSN_FORMAT,
                          posn->accnt.rec->id, posn->contr.rec->id, posn->settl_day,
                          posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    } else {
        n = dbr_pack_lenf(POSN_FORMAT,
                          posn->accnt.id_only, posn->contr.id_only, posn->settl_day,
                          posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    }
    return n;
}

DBR_EXTERN char*
elm_proto_write_posn(char* buf, const struct DbrPosn* posn, DbrBool enriched)
{
    if (enriched) {
        buf = dbr_packf(buf, POSN_FORMAT,
                        posn->accnt.rec->id, posn->contr.rec->id, posn->settl_day,
                        posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    } else {
        buf = dbr_packf(buf, POSN_FORMAT,
                        posn->accnt.id_only, posn->contr.id_only, posn->settl_day,
                        posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    }
    return buf;
}

DBR_EXTERN const char*
elm_proto_read_posn(const char* buf, struct DbrPosn* posn)
{
    return dbr_unpackf(buf, POSN_FORMAT,
                       &posn->accnt.id_only, &posn->contr.id_only, &posn->settl_day,
                       &posn->buy_licks, &posn->buy_lots, &posn->sell_licks, &posn->sell_lots);
}

DBR_EXTERN size_t
elm_proto_view_len(const struct DbrView* view, DbrBool enriched)
{
    size_t n;
    if (enriched) {
        n = dbr_pack_lenf(VIEW_FORMAT,
                          view->contr.rec->id, view->settl_day,
                          view->bid_ticks[0], view->bid_lots[0], view->bid_count[0],
                          view->offer_ticks[0], view->offer_lots[0], view->offer_count[0],
                          view->bid_ticks[1], view->bid_lots[1], view->bid_count[1],
                          view->offer_ticks[1], view->offer_lots[1], view->offer_count[1],
                          view->bid_ticks[2], view->bid_lots[2], view->bid_count[2],
                          view->offer_ticks[2], view->offer_lots[2], view->offer_count[2],
                          view->created);
    } else {
        n = dbr_pack_lenf(VIEW_FORMAT,
                          view->contr.id_only, view->settl_day,
                          view->bid_ticks[0], view->bid_lots[0], view->bid_count[0],
                          view->offer_ticks[0], view->offer_lots[0], view->offer_count[0],
                          view->bid_ticks[1], view->bid_lots[1], view->bid_count[1],
                          view->offer_ticks[1], view->offer_lots[1], view->offer_count[1],
                          view->bid_ticks[2], view->bid_lots[2], view->bid_count[2],
                          view->offer_ticks[2], view->offer_lots[2], view->offer_count[2],
                          view->created);
    }
    return n;
}

DBR_EXTERN char*
elm_proto_write_view(char* buf, const struct DbrView* view, DbrBool enriched)
{
    if (enriched) {
        buf = dbr_packf(buf, VIEW_FORMAT,
                        view->contr.rec->id, view->settl_day,
                        view->bid_ticks[0], view->bid_lots[0], view->bid_count[0],
                        view->offer_ticks[0], view->offer_lots[0], view->offer_count[0],
                        view->bid_ticks[1], view->bid_lots[1], view->bid_count[1],
                        view->offer_ticks[1], view->offer_lots[1], view->offer_count[1],
                        view->bid_ticks[2], view->bid_lots[2], view->bid_count[2],
                        view->offer_ticks[2], view->offer_lots[2], view->offer_count[2],
                        view->created);

    } else {
        buf = dbr_packf(buf, VIEW_FORMAT,
                        view->contr.id_only, view->settl_day,
                        view->bid_ticks[0], view->bid_lots[0], view->bid_count[0],
                        view->offer_ticks[0], view->offer_lots[0], view->offer_count[0],
                        view->bid_ticks[1], view->bid_lots[1], view->bid_count[1],
                        view->offer_ticks[1], view->offer_lots[1], view->offer_count[1],
                        view->bid_ticks[2], view->bid_lots[2], view->bid_count[2],
                        view->offer_ticks[2], view->offer_lots[2], view->offer_count[2],
                        view->created);
    }
    return buf;
}

DBR_EXTERN const char*
elm_proto_read_view(const char* buf, struct DbrView* view)
{
    return dbr_unpackf(buf, VIEW_FORMAT,
                       &view->contr.id_only, &view->settl_day,
                       &view->bid_ticks[0], &view->bid_lots[0], &view->bid_count[0],
                       &view->offer_ticks[0], &view->offer_lots[0], &view->offer_count[0],
                       &view->bid_ticks[1], &view->bid_lots[1], &view->bid_count[1],
                       &view->offer_ticks[1], &view->offer_lots[1], &view->offer_count[1],
                       &view->bid_ticks[2], &view->bid_lots[2], &view->bid_count[2],
                       &view->offer_ticks[2], &view->offer_lots[2], &view->offer_count[2],
                       &view->created);
}

DBR_API size_t
dbr_proto_accnt_len(const struct DbrRec* rec)
{
    return elm_proto_accnt_len(rec);
}

DBR_API char*
dbr_proto_write_accnt(char* buf, const struct DbrRec* rec)
{
    return elm_proto_write_accnt(buf, rec);
}

DBR_API const char*
dbr_proto_read_accnt(const char* buf, struct DbrRec* rec)
{
    return elm_proto_read_accnt(buf, rec);
}

DBR_API size_t
dbr_proto_contr_len(const struct DbrRec* rec)
{
    return elm_proto_contr_len(rec);
}

DBR_API char*
dbr_proto_write_contr(char* buf, const struct DbrRec* rec)
{
    return elm_proto_write_contr(buf, rec);
}

DBR_API const char*
dbr_proto_read_contr(const char* buf, struct DbrRec* rec)
{
    return elm_proto_read_contr(buf, rec);
}

DBR_API size_t
dbr_proto_perm_len(const struct DbrPerm* perm, DbrBool enriched)
{
    return elm_proto_perm_len(perm, enriched);
}

DBR_API char*
dbr_proto_write_perm(char* buf, const struct DbrPerm* perm, DbrBool enriched)
{
    return elm_proto_write_perm(buf, perm, enriched);
}

DBR_API const char*
dbr_proto_read_perm(const char* buf, struct DbrPerm* perm)
{
    return elm_proto_read_perm(buf, perm);
}

DBR_API size_t
dbr_proto_order_len(const struct DbrOrder* order, DbrBool enriched)
{
    return elm_proto_order_len(order, enriched);
}

DBR_API char*
dbr_proto_write_order(char* buf, const struct DbrOrder* order, DbrBool enriched)
{
    return elm_proto_write_order(buf, order, enriched);
}

DBR_API const char*
dbr_proto_read_order(const char* buf, struct DbrOrder* order)
{
    return elm_proto_read_order(buf, order);
}

DBR_API size_t
dbr_proto_exec_len(const struct DbrExec* exec, DbrBool enriched)
{
    return elm_proto_exec_len(exec, enriched);
}

DBR_API char*
dbr_proto_write_exec(char* buf, const struct DbrExec* exec, DbrBool enriched)
{
    return elm_proto_write_exec(buf, exec, enriched);
}

DBR_API const char*
dbr_proto_read_exec(const char* buf, struct DbrExec* exec)
{
    return elm_proto_read_exec(buf, exec);
}

DBR_API size_t
dbr_proto_posn_len(const struct DbrPosn* posn, DbrBool enriched)
{
    return elm_proto_posn_len(posn, enriched);
}

DBR_API char*
dbr_proto_write_posn(char* buf, const struct DbrPosn* posn, DbrBool enriched)
{
    return elm_proto_write_posn(buf, posn, enriched);
}

DBR_API const char*
dbr_proto_read_posn(const char* buf, struct DbrPosn* posn)
{
    return elm_proto_read_posn(buf, posn);
}

DBR_API size_t
dbr_proto_view_len(const struct DbrView* view, DbrBool enriched)
{
    return elm_proto_view_len(view, enriched);
}

DBR_API char*
dbr_proto_write_view(char* buf, const struct DbrView* view, DbrBool enriched)
{
    return elm_proto_write_view(buf, view, enriched);
}

DBR_API const char*
dbr_proto_read_view(const char* buf, struct DbrView* view)
{
    return elm_proto_read_view(buf, view);
}
