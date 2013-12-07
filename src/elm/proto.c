/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
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
#include <dbr/proto.h>

#include <dbr/conv.h>
#include <dbr/err.h>
#include <dbr/pack.h>
#include <dbr/types.h>

#include <stdlib.h> // abort()

static const char TRADER_FORMAT[] = "lmss";
static const char ACCNT_FORMAT[] = "lmss";
static const char CONTR_FORMAT[] = "lmsmmmiiiiill";
static const char ORDER_FORMAT[] = "llllisiilllllllll";
static const char EXEC_FORMAT[] = "lllllisiillllllllill";
static const char MEMB_FORMAT[] = "ll";
static const char POSN_FORMAT[] = "lllllll";

static const char INSERT_EXEC_FORMAT[] = "lllllisiillllllllill";
static const char ACK_TRADE_FORMAT[] = "ll";

DBR_API size_t
dbr_trader_len(const struct DbrRec* rec)
{
    return dbr_packlenf(TRADER_FORMAT,
                        rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                        DBR_EMAIL_MAX, rec->trader.email);
}

DBR_API char*
dbr_write_trader(char* buf, const struct DbrRec* rec)
{
    return dbr_packf(buf, TRADER_FORMAT,
                     rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                     DBR_EMAIL_MAX, rec->trader.email);
}

DBR_API const char*
dbr_read_trader(const char* buf, struct DbrRec* rec)
{
    dbr_rec_init(rec);
    rec->type = DBR_TRADER;
    rec->trader.state = NULL;
    return dbr_unpackf(buf, TRADER_FORMAT,
                       &rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                       DBR_EMAIL_MAX, rec->trader.email);
}

DBR_API size_t
dbr_accnt_len(const struct DbrRec* rec)
{
    return dbr_packlenf(ACCNT_FORMAT,
                        rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                        DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_API char*
dbr_write_accnt(char* buf, const struct DbrRec* rec)
{
    return dbr_packf(buf, ACCNT_FORMAT,
                     rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                     DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_API const char*
dbr_read_accnt(const char* buf, struct DbrRec* rec)
{
    dbr_rec_init(rec);
    rec->type = DBR_ACCNT;
    rec->accnt.state = NULL;
    return dbr_unpackf(buf, ACCNT_FORMAT,
                       &rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                       DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_API size_t
dbr_contr_len(const struct DbrRec* rec)
{
    return dbr_packlenf(CONTR_FORMAT,
                        rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                        rec->contr.asset_type, rec->contr.asset, rec->contr.ccy,
                        rec->contr.tick_numer, rec->contr.tick_denom, rec->contr.lot_numer,
                        rec->contr.lot_denom, rec->contr.pip_dp, rec->contr.min_lots,
                        rec->contr.max_lots);
}

DBR_API char*
dbr_write_contr(char* buf, const struct DbrRec* rec)
{
    return dbr_packf(buf, CONTR_FORMAT,
                     rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->display,
                     rec->contr.asset_type, rec->contr.asset, rec->contr.ccy,
                     rec->contr.tick_numer, rec->contr.tick_denom, rec->contr.lot_numer,
                     rec->contr.lot_denom, rec->contr.pip_dp, rec->contr.min_lots,
                     rec->contr.max_lots);
}

DBR_API const char*
dbr_read_contr(const char* buf, struct DbrRec* rec)
{
    dbr_rec_init(rec);
    rec->type = DBR_CONTR;
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

DBR_API size_t
dbr_rec_len(const struct DbrRec* rec)
{
    size_t n = dbr_packleni(rec->type);
    switch (rec->type) {
    case DBR_TRADER:
        n += dbr_trader_len(rec);
        break;
    case DBR_ACCNT:
        n += dbr_accnt_len(rec);
        break;
    case DBR_CONTR:
        n += dbr_contr_len(rec);
        break;
    default:
        abort();
    }
    return n;
}

DBR_API char*
dbr_write_rec(char* buf, const struct DbrRec* rec)
{
    buf = dbr_packi(buf, rec->type);
    switch (rec->type) {
    case DBR_TRADER:
        buf = dbr_write_trader(buf, rec);
        break;
    case DBR_ACCNT:
        buf = dbr_write_accnt(buf, rec);
        break;
    case DBR_CONTR:
        buf = dbr_write_contr(buf, rec);
        break;
    default:
        abort();
    }
    return buf;
}

DBR_API const char*
dbr_read_rec(const char* buf, struct DbrRec* rec)
{
    buf = dbr_unpacki(buf, &rec->type);
    switch (rec->type) {
    case DBR_TRADER:
        buf = dbr_read_trader(buf, rec);
        break;
    case DBR_ACCNT:
        buf = dbr_read_accnt(buf, rec);
        break;
    case DBR_CONTR:
        buf = dbr_read_contr(buf, rec);
        break;
    default:
        dbr_err_setf(DBR_EIO, "invalid type %d", rec->type);
        buf = NULL;
    }
    return buf;
}

DBR_API size_t
dbr_order_len(const struct DbrOrder* order, DbrBool enriched)
{
    size_t n;
    if (enriched) {
        n = dbr_packlenf(ORDER_FORMAT,
                         order->id, order->c.trader.rec->id, order->c.accnt.rec->id,
                         order->c.contr.rec->id, order->c.settl_date, DBR_REF_MAX, order->c.ref,
                         order->c.state, order->c.action, order->c.ticks, order->c.lots,
                         order->c.resd, order->c.exec, order->c.last_ticks, order->c.last_lots,
                         order->c.min_lots, order->created, order->modified);
    } else {
        n = dbr_packlenf(ORDER_FORMAT,
                         order->id, order->c.trader.id_only, order->c.accnt.id_only,
                         order->c.contr.id_only, order->c.settl_date, DBR_REF_MAX, order->c.ref,
                         order->c.state, order->c.action, order->c.ticks, order->c.lots,
                         order->c.resd, order->c.exec, order->c.last_ticks, order->c.last_lots,
                         order->c.min_lots, order->created, order->modified);
    }
    return n;
}

DBR_API char*
dbr_write_order(char* buf, const struct DbrOrder* order, DbrBool enriched)
{
    if (enriched) {
        buf = dbr_packf(buf, ORDER_FORMAT,
                        order->id, order->c.trader.rec->id, order->c.accnt.rec->id,
                        order->c.contr.rec->id, order->c.settl_date, DBR_REF_MAX,
                        order->c.ref, order->c.state, order->c.action, order->c.ticks,
                        order->c.lots, order->c.resd, order->c.exec, order->c.last_ticks,
                        order->c.last_lots, order->c.min_lots, order->created, order->modified);
    } else {
        buf = dbr_packf(buf, ORDER_FORMAT,
                        order->id, order->c.trader.id_only, order->c.accnt.id_only,
                        order->c.contr.id_only, order->c.settl_date, DBR_REF_MAX,
                        order->c.ref, order->c.state, order->c.action, order->c.ticks,
                        order->c.lots, order->c.resd, order->c.exec, order->c.last_ticks,
                        order->c.last_lots, order->c.min_lots, order->created, order->modified);
    }
    return buf;
}

DBR_API const char*
dbr_read_order(const char* buf, struct DbrOrder* order)
{
    dbr_order_init(order);
    return dbr_unpackf(buf, ORDER_FORMAT,
                       &order->id, &order->c.trader.id_only, &order->c.accnt.id_only,
                       &order->c.contr.id_only, &order->c.settl_date, DBR_REF_MAX,
                       order->c.ref, &order->c.state, &order->c.action, &order->c.ticks,
                       &order->c.lots, &order->c.resd, &order->c.exec, &order->c.last_ticks,
                       &order->c.last_lots, &order->c.min_lots, &order->created, &order->modified);
}

DBR_API size_t
dbr_exec_len(const struct DbrExec* exec, DbrBool enriched)
{
    size_t n;
    if (enriched) {
        n = dbr_packlenf(EXEC_FORMAT,
                         exec->id, exec->order, exec->c.trader.rec->id, exec->c.accnt.rec->id,
                         exec->c.contr.rec->id, exec->c.settl_date, DBR_REF_MAX, exec->c.ref,
                         exec->c.state, exec->c.action, exec->c.ticks, exec->c.lots,
                         exec->c.resd, exec->c.exec, exec->c.last_ticks, exec->c.last_lots,
                         exec->c.min_lots, exec->match, exec->role, exec->cpty.rec->id,
                         exec->created);
    } else {
        n = dbr_packlenf(EXEC_FORMAT,
                         exec->id, exec->order, exec->c.trader.id_only, exec->c.accnt.id_only,
                         exec->c.contr.id_only, exec->c.settl_date, DBR_REF_MAX, exec->c.ref,
                         exec->c.state, exec->c.action, exec->c.ticks, exec->c.lots,
                         exec->c.resd, exec->c.exec, exec->c.last_ticks, exec->c.last_lots,
                         exec->c.min_lots, exec->match, exec->role, exec->cpty.id_only,
                         exec->created);
    }
    return n;
}

DBR_API char*
dbr_write_exec(char* buf, const struct DbrExec* exec, DbrBool enriched)
{
    if (enriched) {
        buf = dbr_packf(buf, EXEC_FORMAT,
                        exec->id, exec->order, exec->c.trader.rec->id, exec->c.accnt.rec->id,
                        exec->c.contr.rec->id, exec->c.settl_date, DBR_REF_MAX, exec->c.ref,
                        exec->c.state, exec->c.action, exec->c.ticks, exec->c.lots,
                        exec->c.resd, exec->c.exec, exec->c.last_ticks, exec->c.last_lots,
                        exec->c.min_lots, exec->match, exec->role, exec->cpty.rec->id,
                        exec->created);
    } else {
        buf = dbr_packf(buf, EXEC_FORMAT,
                        exec->id, exec->order, exec->c.trader.id_only, exec->c.accnt.id_only,
                        exec->c.contr.id_only, exec->c.settl_date, DBR_REF_MAX, exec->c.ref,
                        exec->c.state, exec->c.action, exec->c.ticks, exec->c.lots,
                        exec->c.resd, exec->c.exec, exec->c.last_ticks, exec->c.last_lots,
                        exec->c.min_lots, exec->match, exec->role, exec->cpty.id_only,
                        exec->created);
    }
    return buf;
}

DBR_API const char*
dbr_read_exec(const char* buf, struct DbrExec* exec)
{
    dbr_exec_init(exec);
    return dbr_unpackf(buf, EXEC_FORMAT,
                       &exec->id, &exec->order, &exec->c.trader.id_only, &exec->c.accnt.id_only,
                       &exec->c.contr.id_only, &exec->c.settl_date, DBR_REF_MAX, exec->c.ref,
                       &exec->c.state, &exec->c.action, &exec->c.ticks, &exec->c.lots,
                       &exec->c.resd, &exec->c.exec, &exec->c.last_ticks, &exec->c.last_lots,
                       &exec->c.min_lots, &exec->match, &exec->role, &exec->cpty.id_only,
                       &exec->created);
}

DBR_API size_t
dbr_memb_len(const struct DbrMemb* memb, DbrBool enriched)
{
    size_t n;
    if (enriched) {
        n = dbr_packlenf(MEMB_FORMAT,
                         memb->trader.rec->id, memb->accnt.rec->id);
    } else {
        n = dbr_packlenf(MEMB_FORMAT,
                         memb->trader.id_only, memb->accnt.id_only);
    }
    return n;
}

DBR_API char*
dbr_write_memb(char* buf, const struct DbrMemb* memb, DbrBool enriched)
{
    if (enriched) {
        buf = dbr_packf(buf, MEMB_FORMAT,
                        memb->trader.rec->id, memb->accnt.rec->id);
    } else {
        buf = dbr_packf(buf, MEMB_FORMAT,
                        memb->trader.id_only, memb->accnt.id_only);
    }
    return buf;
}

DBR_API const char*
dbr_read_memb(const char* buf, struct DbrMemb* memb)
{
    dbr_memb_init(memb);
    return dbr_unpackf(buf, MEMB_FORMAT, &memb->trader.id_only, &memb->accnt.id_only);
}

DBR_API size_t
dbr_posn_len(const struct DbrPosn* posn, DbrBool enriched)
{
    size_t n;
    if (enriched) {
        n = dbr_packlenf(POSN_FORMAT,
                         posn->accnt.rec->id, posn->contr.rec->id, posn->settl_date,
                         posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    } else {
        n = dbr_packlenf(POSN_FORMAT,
                         posn->accnt.id_only, posn->contr.id_only, posn->settl_date,
                         posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    }
    return n;
}

DBR_API char*
dbr_write_posn(char* buf, const struct DbrPosn* posn, DbrBool enriched)
{
    if (enriched) {
        buf = dbr_packf(buf, POSN_FORMAT,
                        posn->accnt.rec->id, posn->contr.rec->id, posn->settl_date,
                        posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    } else {
        buf = dbr_packf(buf, POSN_FORMAT,
                        posn->accnt.id_only, posn->contr.id_only, posn->settl_date,
                        posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
    }
    return buf;
}

DBR_API const char*
dbr_read_posn(const char* buf, struct DbrPosn* posn)
{
    dbr_posn_init(posn);
    return dbr_unpackf(buf, POSN_FORMAT,
                       &posn->accnt.id_only, &posn->contr.id_only, &posn->settl_date,
                       &posn->buy_licks, &posn->buy_lots, &posn->sell_licks, &posn->sell_lots);
}
