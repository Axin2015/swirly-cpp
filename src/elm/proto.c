/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "proto.h"

#include <sc/elm/conv.h>
#include <sc/elm/types.h>

#include <sc/ash/err.h>
#include <sc/ash/pack.h>

#include <stdlib.h> // abort()

static const char ACCNT_FORMAT[] = "lmss";
static const char CONTR_FORMAT[] = "lmsmmmiiiiill";
static const char PERM_FORMAT[] = "ll";
static const char ORDER_FORMAT[] = "llllisiilllllllll";
static const char EXEC_FORMAT[] = "lllllisiillllllllill";
static const char POSN_FORMAT[] = "lllllll";
static const char VIEW_FORMAT[] = "lillzllzllzllzllzllzl";

SC_EXTERN size_t
elm_proto_accnt_len(const struct ScRec* rec)
{
    return sc_pack_lenf(ACCNT_FORMAT,
                         rec->id, rec->mnem, SC_DISPLAY_MAX, rec->display,
                         SC_EMAIL_MAX, rec->accnt.email);
}

SC_EXTERN char*
elm_proto_write_accnt(char* buf, const struct ScRec* rec)
{
    return sc_packf(buf, ACCNT_FORMAT,
                     rec->id, rec->mnem, SC_DISPLAY_MAX, rec->display,
                     SC_EMAIL_MAX, rec->accnt.email);
}

SC_EXTERN const char*
elm_proto_read_accnt(const char* buf, struct ScRec* rec)
{
    rec->type = SC_ENTITY_ACCNT;
    rec->accnt.state = NULL;
    return sc_unpackf(buf, ACCNT_FORMAT,
                       &rec->id, rec->mnem, SC_DISPLAY_MAX, rec->display,
                       SC_EMAIL_MAX, rec->accnt.email);
}

SC_EXTERN size_t
elm_proto_contr_len(const struct ScRec* rec)
{
    return sc_pack_lenf(CONTR_FORMAT,
                         rec->id, rec->mnem, SC_DISPLAY_MAX, rec->display,
                         rec->contr.asset_type, rec->contr.asset, rec->contr.ccy,
                         rec->contr.tick_numer, rec->contr.tick_denom, rec->contr.lot_numer,
                         rec->contr.lot_denom, rec->contr.pip_dp, rec->contr.min_lots,
                         rec->contr.max_lots);
}

SC_EXTERN char*
elm_proto_write_contr(char* buf, const struct ScRec* rec)
{
    return sc_packf(buf, CONTR_FORMAT,
                     rec->id, rec->mnem, SC_DISPLAY_MAX, rec->display,
                     rec->contr.asset_type, rec->contr.asset, rec->contr.ccy,
                     rec->contr.tick_numer, rec->contr.tick_denom, rec->contr.lot_numer,
                     rec->contr.lot_denom, rec->contr.pip_dp, rec->contr.min_lots,
                     rec->contr.max_lots);
}

SC_EXTERN const char*
elm_proto_read_contr(const char* buf, struct ScRec* rec)
{
    rec->type = SC_ENTITY_CONTR;
    const char* end = sc_unpackf(buf, CONTR_FORMAT,
                                  &rec->id, rec->mnem, SC_DISPLAY_MAX, rec->display,
                                  rec->contr.asset_type, rec->contr.asset, rec->contr.ccy,
                                  &rec->contr.tick_numer, &rec->contr.tick_denom,
                                  &rec->contr.lot_numer, &rec->contr.lot_denom,
                                  &rec->contr.pip_dp, &rec->contr.min_lots, &rec->contr.max_lots);

    // Derive transient fields.
    rec->contr.price_inc = sc_fract_to_real(rec->contr.tick_numer, rec->contr.tick_denom);
    rec->contr.qty_inc = sc_fract_to_real(rec->contr.lot_numer, rec->contr.lot_denom);
    rec->contr.price_dp = sc_real_to_dp(rec->contr.price_inc);
    rec->contr.qty_dp = sc_real_to_dp(rec->contr.qty_inc);
    return end;
}

SC_EXTERN size_t
elm_proto_perm_len(const struct ScPerm* perm, ScBool enriched)
{
    size_t n;
    if (enriched) {
        n = sc_pack_lenf(PERM_FORMAT,
                          perm->giveup.rec->id, perm->trader.rec->id);
    } else {
        n = sc_pack_lenf(PERM_FORMAT,
                          perm->giveup.id_only, perm->trader.id_only);
    }
    return n;
}

SC_EXTERN char*
elm_proto_write_perm(char* buf, const struct ScPerm* perm, ScBool enriched)
{
    if (enriched) {
        buf = sc_packf(buf, PERM_FORMAT,
                        perm->giveup.rec->id, perm->trader.rec->id);
    } else {
        buf = sc_packf(buf, PERM_FORMAT,
                        perm->giveup.id_only, perm->trader.id_only);
    }
    return buf;
}

SC_EXTERN const char*
elm_proto_read_perm(const char* buf, struct ScPerm* perm)
{
    return sc_unpackf(buf, PERM_FORMAT, &perm->giveup.id_only, &perm->trader.id_only);
}

SC_EXTERN size_t
elm_proto_order_len(const struct ScOrder* order, ScBool enriched)
{
    size_t n;
    if (enriched) {
        n = sc_pack_lenf(ORDER_FORMAT,
                          order->id, order->i.trader.rec->id, order->i.giveup.rec->id,
                          order->i.contr.rec->id, order->i.settl_day, SC_REF_MAX, order->i.ref,
                          order->i.state, order->i.action, order->i.ticks, order->i.lots,
                          order->i.resd, order->i.exec, order->i.last_ticks, order->i.last_lots,
                          order->i.min_lots, order->created, order->modified);
    } else {
        n = sc_pack_lenf(ORDER_FORMAT,
                          order->id, order->i.trader.id_only, order->i.giveup.id_only,
                          order->i.contr.id_only, order->i.settl_day, SC_REF_MAX, order->i.ref,
                          order->i.state, order->i.action, order->i.ticks, order->i.lots,
                          order->i.resd, order->i.exec, order->i.last_ticks, order->i.last_lots,
                          order->i.min_lots, order->created, order->modified);
    }
    return n;
}

SC_EXTERN char*
elm_proto_write_order(char* buf, const struct ScOrder* order, ScBool enriched)
{
    if (enriched) {
        buf = sc_packf(buf, ORDER_FORMAT,
                        order->id, order->i.trader.rec->id, order->i.giveup.rec->id,
                        order->i.contr.rec->id, order->i.settl_day, SC_REF_MAX,
                        order->i.ref, order->i.state, order->i.action, order->i.ticks,
                        order->i.lots, order->i.resd, order->i.exec, order->i.last_ticks,
                        order->i.last_lots, order->i.min_lots, order->created, order->modified);
    } else {
        buf = sc_packf(buf, ORDER_FORMAT,
                        order->id, order->i.trader.id_only, order->i.giveup.id_only,
                        order->i.contr.id_only, order->i.settl_day, SC_REF_MAX,
                        order->i.ref, order->i.state, order->i.action, order->i.ticks,
                        order->i.lots, order->i.resd, order->i.exec, order->i.last_ticks,
                        order->i.last_lots, order->i.min_lots, order->created, order->modified);
    }
    return buf;
}

SC_EXTERN const char*
elm_proto_read_order(const char* buf, struct ScOrder* order)
{
    return sc_unpackf(buf, ORDER_FORMAT,
                       &order->id, &order->i.trader.id_only, &order->i.giveup.id_only,
                       &order->i.contr.id_only, &order->i.settl_day, SC_REF_MAX,
                       order->i.ref, &order->i.state, &order->i.action, &order->i.ticks,
                       &order->i.lots, &order->i.resd, &order->i.exec, &order->i.last_ticks,
                       &order->i.last_lots, &order->i.min_lots, &order->created, &order->modified);
}

SC_EXTERN size_t
elm_proto_exec_len(const struct ScExec* exec, ScBool enriched)
{
    size_t n;
    if (enriched) {
        const ScIden cpty = exec->cpty.rec ? exec->cpty.rec->id : 0;
        n = sc_pack_lenf(EXEC_FORMAT,
                          exec->id, exec->order, exec->i.trader.rec->id, exec->i.giveup.rec->id,
                          exec->i.contr.rec->id, exec->i.settl_day, SC_REF_MAX, exec->i.ref,
                          exec->i.state, exec->i.action, exec->i.ticks, exec->i.lots,
                          exec->i.resd, exec->i.exec, exec->i.last_ticks, exec->i.last_lots,
                          exec->i.min_lots, exec->match, exec->role, cpty, exec->created);
    } else {
        n = sc_pack_lenf(EXEC_FORMAT,
                          exec->id, exec->order, exec->i.trader.id_only, exec->i.giveup.id_only,
                          exec->i.contr.id_only, exec->i.settl_day, SC_REF_MAX, exec->i.ref,
                          exec->i.state, exec->i.action, exec->i.ticks, exec->i.lots,
                          exec->i.resd, exec->i.exec, exec->i.last_ticks, exec->i.last_lots,
                          exec->i.min_lots, exec->match, exec->role, exec->cpty.id_only,
                          exec->created);
    }
    return n;
}

SC_EXTERN char*
elm_proto_write_exec(char* buf, const struct ScExec* exec, ScBool enriched)
{
    if (enriched) {
        const ScIden cpty = exec->cpty.rec ? exec->cpty.rec->id : 0;
        buf = sc_packf(buf, EXEC_FORMAT,
                        exec->id, exec->order, exec->i.trader.rec->id, exec->i.giveup.rec->id,
                        exec->i.contr.rec->id, exec->i.settl_day, SC_REF_MAX, exec->i.ref,
                        exec->i.state, exec->i.action, exec->i.ticks, exec->i.lots,
                        exec->i.resd, exec->i.exec, exec->i.last_ticks, exec->i.last_lots,
                        exec->i.min_lots, exec->match, exec->role, cpty, exec->created);
    } else {
        buf = sc_packf(buf, EXEC_FORMAT,
                        exec->id, exec->order, exec->i.trader.id_only, exec->i.giveup.id_only,
                        exec->i.contr.id_only, exec->i.settl_day, SC_REF_MAX, exec->i.ref,
                        exec->i.state, exec->i.action, exec->i.ticks, exec->i.lots,
                        exec->i.resd, exec->i.exec, exec->i.last_ticks, exec->i.last_lots,
                        exec->i.min_lots, exec->match, exec->role, exec->cpty.id_only,
                        exec->created);
    }
    return buf;
}

SC_EXTERN const char*
elm_proto_read_exec(const char* buf, struct ScExec* exec)
{
    return sc_unpackf(buf, EXEC_FORMAT,
                       &exec->id, &exec->order, &exec->i.trader.id_only, &exec->i.giveup.id_only,
                       &exec->i.contr.id_only, &exec->i.settl_day, SC_REF_MAX, exec->i.ref,
                       &exec->i.state, &exec->i.action, &exec->i.ticks, &exec->i.lots,
                       &exec->i.resd, &exec->i.exec, &exec->i.last_ticks, &exec->i.last_lots,
                       &exec->i.min_lots, &exec->match, &exec->role, &exec->cpty.id_only,
                       &exec->created);
}

SC_EXTERN size_t
elm_proto_posn_len(const struct ScPosn* posn, ScBool enriched)
{
    size_t n;
    if (enriched) {
        n = sc_pack_lenf(POSN_FORMAT,
                          posn->accnt.rec->id, posn->contr.rec->id, posn->settl_day,
                          posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    } else {
        n = sc_pack_lenf(POSN_FORMAT,
                          posn->accnt.id_only, posn->contr.id_only, posn->settl_day,
                          posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    }
    return n;
}

SC_EXTERN char*
elm_proto_write_posn(char* buf, const struct ScPosn* posn, ScBool enriched)
{
    if (enriched) {
        buf = sc_packf(buf, POSN_FORMAT,
                        posn->accnt.rec->id, posn->contr.rec->id, posn->settl_day,
                        posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    } else {
        buf = sc_packf(buf, POSN_FORMAT,
                        posn->accnt.id_only, posn->contr.id_only, posn->settl_day,
                        posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    }
    return buf;
}

SC_EXTERN const char*
elm_proto_read_posn(const char* buf, struct ScPosn* posn)
{
    return sc_unpackf(buf, POSN_FORMAT,
                       &posn->accnt.id_only, &posn->contr.id_only, &posn->settl_day,
                       &posn->buy_licks, &posn->buy_lots, &posn->sell_licks, &posn->sell_lots);
}

SC_EXTERN size_t
elm_proto_view_len(const struct ScView* view, ScBool enriched)
{
    size_t n;
    if (enriched) {
        n = sc_pack_lenf(VIEW_FORMAT,
                          view->contr.rec->id, view->settl_day,
                          view->bid_ticks[0], view->bid_lots[0], view->bid_count[0],
                          view->offer_ticks[0], view->offer_lots[0], view->offer_count[0],
                          view->bid_ticks[1], view->bid_lots[1], view->bid_count[1],
                          view->offer_ticks[1], view->offer_lots[1], view->offer_count[1],
                          view->bid_ticks[2], view->bid_lots[2], view->bid_count[2],
                          view->offer_ticks[2], view->offer_lots[2], view->offer_count[2],
                          view->created);
    } else {
        n = sc_pack_lenf(VIEW_FORMAT,
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

SC_EXTERN char*
elm_proto_write_view(char* buf, const struct ScView* view, ScBool enriched)
{
    if (enriched) {
        buf = sc_packf(buf, VIEW_FORMAT,
                        view->contr.rec->id, view->settl_day,
                        view->bid_ticks[0], view->bid_lots[0], view->bid_count[0],
                        view->offer_ticks[0], view->offer_lots[0], view->offer_count[0],
                        view->bid_ticks[1], view->bid_lots[1], view->bid_count[1],
                        view->offer_ticks[1], view->offer_lots[1], view->offer_count[1],
                        view->bid_ticks[2], view->bid_lots[2], view->bid_count[2],
                        view->offer_ticks[2], view->offer_lots[2], view->offer_count[2],
                        view->created);

    } else {
        buf = sc_packf(buf, VIEW_FORMAT,
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

SC_EXTERN const char*
elm_proto_read_view(const char* buf, struct ScView* view)
{
    return sc_unpackf(buf, VIEW_FORMAT,
                       &view->contr.id_only, &view->settl_day,
                       &view->bid_ticks[0], &view->bid_lots[0], &view->bid_count[0],
                       &view->offer_ticks[0], &view->offer_lots[0], &view->offer_count[0],
                       &view->bid_ticks[1], &view->bid_lots[1], &view->bid_count[1],
                       &view->offer_ticks[1], &view->offer_lots[1], &view->offer_count[1],
                       &view->bid_ticks[2], &view->bid_lots[2], &view->bid_count[2],
                       &view->offer_ticks[2], &view->offer_lots[2], &view->offer_count[2],
                       &view->created);
}

SC_API size_t
sc_proto_accnt_len(const struct ScRec* rec)
{
    return elm_proto_accnt_len(rec);
}

SC_API char*
sc_proto_write_accnt(char* buf, const struct ScRec* rec)
{
    return elm_proto_write_accnt(buf, rec);
}

SC_API const char*
sc_proto_read_accnt(const char* buf, struct ScRec* rec)
{
    return elm_proto_read_accnt(buf, rec);
}

SC_API size_t
sc_proto_contr_len(const struct ScRec* rec)
{
    return elm_proto_contr_len(rec);
}

SC_API char*
sc_proto_write_contr(char* buf, const struct ScRec* rec)
{
    return elm_proto_write_contr(buf, rec);
}

SC_API const char*
sc_proto_read_contr(const char* buf, struct ScRec* rec)
{
    return elm_proto_read_contr(buf, rec);
}

SC_API size_t
sc_proto_perm_len(const struct ScPerm* perm, ScBool enriched)
{
    return elm_proto_perm_len(perm, enriched);
}

SC_API char*
sc_proto_write_perm(char* buf, const struct ScPerm* perm, ScBool enriched)
{
    return elm_proto_write_perm(buf, perm, enriched);
}

SC_API const char*
sc_proto_read_perm(const char* buf, struct ScPerm* perm)
{
    return elm_proto_read_perm(buf, perm);
}

SC_API size_t
sc_proto_order_len(const struct ScOrder* order, ScBool enriched)
{
    return elm_proto_order_len(order, enriched);
}

SC_API char*
sc_proto_write_order(char* buf, const struct ScOrder* order, ScBool enriched)
{
    return elm_proto_write_order(buf, order, enriched);
}

SC_API const char*
sc_proto_read_order(const char* buf, struct ScOrder* order)
{
    return elm_proto_read_order(buf, order);
}

SC_API size_t
sc_proto_exec_len(const struct ScExec* exec, ScBool enriched)
{
    return elm_proto_exec_len(exec, enriched);
}

SC_API char*
sc_proto_write_exec(char* buf, const struct ScExec* exec, ScBool enriched)
{
    return elm_proto_write_exec(buf, exec, enriched);
}

SC_API const char*
sc_proto_read_exec(const char* buf, struct ScExec* exec)
{
    return elm_proto_read_exec(buf, exec);
}

SC_API size_t
sc_proto_posn_len(const struct ScPosn* posn, ScBool enriched)
{
    return elm_proto_posn_len(posn, enriched);
}

SC_API char*
sc_proto_write_posn(char* buf, const struct ScPosn* posn, ScBool enriched)
{
    return elm_proto_write_posn(buf, posn, enriched);
}

SC_API const char*
sc_proto_read_posn(const char* buf, struct ScPosn* posn)
{
    return elm_proto_read_posn(buf, posn);
}

SC_API size_t
sc_proto_view_len(const struct ScView* view, ScBool enriched)
{
    return elm_proto_view_len(view, enriched);
}

SC_API char*
sc_proto_write_view(char* buf, const struct ScView* view, ScBool enriched)
{
    return elm_proto_write_view(buf, view, enriched);
}

SC_API const char*
sc_proto_read_view(const char* buf, struct ScView* view)
{
    return elm_proto_read_view(buf, view);
}
