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
static const char ORDER_FORMAT[] = "liilllisillllllll";
static const char MEMB_FORMAT[] = "ll";
static const char TRADE_FORMAT[] = "lllilllisliillllll";
static const char POSN_FORMAT[] = "lllllll";

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
    rec->type = DBR_TRADER;
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
    rec->type = DBR_ACCNT;
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
        dbr_err_set(DBR_EIO, "invalid type %d", rec->type);
        buf = NULL;
    }
    return buf;
}

DBR_API size_t
dbr_order_len(const struct DbrOrder* order)
{
    return dbr_packlenf(ORDER_FORMAT,
                        order->id, order->rev, order->status, order->trader.id, order->accnt.id,
                        order->contr.id, order->settl_date, DBR_REF_MAX, order->ref, order->action,
                        order->ticks, order->resd, order->exec, order->lots, order->min,
                        order->flags, order->created, order->modified);
}

DBR_API char*
dbr_write_order(char* buf, const struct DbrOrder* order)
{
    return dbr_packf(buf, ORDER_FORMAT,
                     order->id, order->rev, order->status, order->trader.id, order->accnt.id,
                     order->contr.id, order->settl_date, DBR_REF_MAX, order->ref, order->action,
                     order->ticks, order->resd, order->exec, order->lots, order->min, order->flags,
                     order->created, order->modified);
}

DBR_API const char*
dbr_read_order(const char* buf, struct DbrOrder* order)
{
    buf = dbr_unpackf(buf, ORDER_FORMAT,
                      &order->id, &order->rev, &order->status, &order->trader.id, &order->accnt.id,
                      &order->contr.id, &order->settl_date, DBR_REF_MAX, order->ref, &order->action,
                      &order->ticks, &order->resd, &order->exec, &order->lots, &order->min,
                      &order->flags, &order->created, &order->modified);
    if (buf)
        order->trader_node_.key = order->id;
    return buf;
}

DBR_API size_t
dbr_memb_len(const struct DbrMemb* memb)
{
    return dbr_packlenf(MEMB_FORMAT,
                        memb->accnt.id, memb->trader.id);
}

DBR_API char*
dbr_write_memb(char* buf, const struct DbrMemb* memb)
{
    return dbr_packf(buf, MEMB_FORMAT,
                     memb->accnt.id, memb->trader.id);
}

DBR_API const char*
dbr_read_memb(const char* buf, struct DbrMemb* memb)
{
    buf = dbr_unpackf(buf, MEMB_FORMAT,
                      &memb->accnt.id, &memb->trader.id);
    if (buf)
        memb->accnt_node_.key = memb->trader.id;
    return buf;
}

DBR_API size_t
dbr_trade_len(const struct DbrTrade* trade)
{
    return dbr_packlenf(TRADE_FORMAT,
                        trade->id, trade->match, trade->order, trade->order_rev,
                        trade->trader.id, trade->accnt.id, trade->contr.id, trade->settl_date,
                        DBR_REF_MAX, trade->ref, trade->cpty.id, trade->role, trade->action,
                        trade->ticks, trade->resd, trade->exec, trade->lots, trade->created,
                        trade->modified);
}

DBR_API char*
dbr_write_trade(char* buf, const struct DbrTrade* trade)
{
    return dbr_packf(buf, TRADE_FORMAT,
                     trade->id, trade->match, trade->order, trade->order_rev,
                     trade->trader.id, trade->accnt.id, trade->contr.id, trade->settl_date,
                     DBR_REF_MAX, trade->ref, trade->cpty.id, trade->role, trade->action,
                     trade->ticks, trade->resd, trade->exec, trade->lots, trade->created,
                     trade->modified);
}

DBR_API const char*
dbr_read_trade(const char* buf, struct DbrTrade* trade)
{
    buf = dbr_unpackf(buf, TRADE_FORMAT,
                      &trade->id, &trade->match, &trade->order, &trade->order_rev,
                      &trade->trader.id, &trade->accnt.id, &trade->contr.id, &trade->settl_date,
                      DBR_REF_MAX, trade->ref, &trade->cpty.id, &trade->role, &trade->action,
                      &trade->ticks, &trade->resd, &trade->exec, &trade->lots, &trade->created,
                      &trade->modified);
    if (buf)
        trade->accnt_node_.key = trade->id;
    return buf;
}

DBR_API size_t
dbr_posn_len(const struct DbrPosn* posn)
{
    return dbr_packlenf(POSN_FORMAT,
                        posn->accnt.id, posn->contr.id, posn->settl_date, posn->buy_licks,
                        posn->buy_lots, posn->sell_licks, posn->sell_lots);
}

DBR_API char*
dbr_write_posn(char* buf, const struct DbrPosn* posn)
{
    return dbr_packf(buf, POSN_FORMAT,
                     posn->accnt.id, posn->contr.id, posn->settl_date, posn->buy_licks,
                     posn->buy_lots, posn->sell_licks, posn->sell_lots);
}

DBR_API const char*
dbr_read_posn(const char* buf, struct DbrPosn* posn)
{
    buf = dbr_unpackf(buf, POSN_FORMAT,
                      &posn->accnt.id, &posn->contr.id, &posn->settl_date, &posn->buy_licks,
                      &posn->buy_lots, &posn->sell_licks, &posn->sell_lots);
    if (buf) {
        // Synthetic key from contract and settlment date.
        const DbrIden key = posn->contr.id * 100000000L + posn->settl_date;
        posn->accnt_node_.key = key;
    }
    return buf;
}
