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
#include <dbr/msg.h>

#include <dbr/pack.h>
#include <dbr/types.h>

static const char INSTR_FORMAT[] = "lmsmmmmiiiiill";
static const char MARKET_FORMAT[] = "lmls";
static const char TRADER_FORMAT[] = "lmss";
static const char ACCNT_FORMAT[] = "lmss";
static const char ORDER_FORMAT[] = "liillslillllllll";
static const char MEMB_FORMAT[] = "ll";
static const char TRADE_FORMAT[] = "lllillslliillllill";
static const char POSN_FORMAT[] = "llllllll";

DBR_API char*
dbr_writeinstr(char* buf, const struct DbrRec* rec)
{
    return dbr_packf(buf, INSTR_FORMAT,
                     rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->instr.display,
                     rec->instr.asset_type, rec->instr.instr_type, rec->instr.asset,
                     rec->instr.ccy, rec->instr.tick_numer, rec->instr.tick_denom,
                     rec->instr.lot_numer, rec->instr.lot_denom, rec->instr.pip_dp,
                     rec->instr.min_lots, rec->instr.max_lots);
}

DBR_API const char*
dbr_readinstr(const char* buf, struct DbrRec* rec)
{
    return dbr_unpackf(buf, INSTR_FORMAT,
                       &rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->instr.display,
                       rec->instr.asset_type, rec->instr.instr_type, rec->instr.asset,
                       rec->instr.ccy, &rec->instr.tick_numer, &rec->instr.tick_denom,
                       &rec->instr.lot_numer, &rec->instr.lot_denom, &rec->instr.pip_dp,
                       &rec->instr.min_lots, &rec->instr.max_lots);
}

DBR_API int
dbr_instrlen(const struct DbrRec* rec)
{
    return dbr_packlenf(INSTR_FORMAT,
                        rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->instr.display,
                        rec->instr.asset_type, rec->instr.instr_type, rec->instr.asset,
                        rec->instr.ccy, rec->instr.tick_numer, rec->instr.tick_denom,
                        rec->instr.lot_numer, rec->instr.lot_denom, rec->instr.pip_dp,
                        rec->instr.min_lots, rec->instr.max_lots);
}

DBR_API char*
dbr_writemarket(char* buf, const struct DbrRec* rec)
{
    return dbr_packf(buf, MARKET_FORMAT,
                     rec->id, rec->mnem, rec->market.instr.id, DBR_TENOR_MAX,
                     rec->market.tenor);
}

DBR_API const char*
dbr_readmarket(const char* buf, struct DbrRec* rec)
{
    return dbr_unpackf(buf, MARKET_FORMAT,
                       &rec->id, rec->mnem, &rec->market.instr.id, DBR_TENOR_MAX,
                       rec->market.tenor);
}

DBR_API int
dbr_marketlen(const struct DbrRec* rec)
{
    return dbr_packlenf(MARKET_FORMAT,
                        rec->id, rec->mnem, rec->market.instr.id, DBR_TENOR_MAX,
                        rec->market.tenor);
}

DBR_API char*
dbr_writetrader(char* buf, const struct DbrRec* rec)
{
    return dbr_packf(buf, TRADER_FORMAT,
                     rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->trader.display,
                     DBR_EMAIL_MAX, rec->trader.email);
}

DBR_API const char*
dbr_readtrader(const char* buf, struct DbrRec* rec)
{
    return dbr_unpackf(buf, TRADER_FORMAT,
                       &rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->trader.display,
                       DBR_EMAIL_MAX, rec->trader.email);
}

DBR_API int
dbr_traderlen(const struct DbrRec* rec)
{
    return dbr_packlenf(TRADER_FORMAT,
                        rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->trader.display,
                        DBR_EMAIL_MAX, rec->trader.email);
}

DBR_API char*
dbr_writeaccnt(char* buf, const struct DbrRec* rec)
{
    return dbr_packf(buf, ACCNT_FORMAT,
                     rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->accnt.display,
                     DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_API const char*
dbr_readaccnt(const char* buf, struct DbrRec* rec)
{
    return dbr_unpackf(buf, ACCNT_FORMAT,
                       &rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->accnt.display,
                       DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_API int
dbr_accntlen(const struct DbrRec* rec)
{
    return dbr_packlenf(ACCNT_FORMAT,
                        rec->id, rec->mnem, DBR_DISPLAY_MAX, rec->accnt.display,
                        DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_API char*
dbr_writeorder(char* buf, const struct DbrOrder* order)
{
    return dbr_packf(buf, ORDER_FORMAT,
                     order->id, order->rev, order->status, order->trader.id, order->accnt.id,
                     DBR_REF_MAX, order->ref, order->market.id, order->action, order->ticks,
                     order->resd, order->exec, order->lots, order->min, order->flags,
                     order->created, order->modified);
}

DBR_API const char*
dbr_readorder(const char* buf, struct DbrOrder* order)
{
    return dbr_unpackf(buf, ORDER_FORMAT,
                       &order->id, &order->rev, &order->status, &order->trader.id, &order->accnt.id,
                       DBR_REF_MAX, order->ref, &order->market.id, &order->action, &order->ticks,
                       &order->resd, &order->exec, &order->lots, &order->min, &order->flags,
                       &order->created, &order->modified);
}

DBR_API int
dbr_orderlen(const struct DbrOrder* order)
{
    return dbr_packlenf(ORDER_FORMAT,
                        order->id, order->rev, order->status, order->trader.id, order->accnt.id,
                        DBR_REF_MAX, order->ref, order->market.id, order->action, order->ticks,
                        order->resd, order->exec, order->lots, order->min, order->flags,
                        order->created, order->modified);
}

DBR_API char*
dbr_writememb(char* buf, const struct DbrMemb* memb)
{
    return dbr_packf(buf, MEMB_FORMAT,
                     memb->accnt.id, memb->trader.id);
}

DBR_API const char*
dbr_readmemb(const char* buf, struct DbrMemb* memb)
{
    return dbr_unpackf(buf, MEMB_FORMAT,
                       &memb->accnt.id, &memb->trader.id);
}

DBR_API int
dbr_memblen(const struct DbrMemb* memb)
{
    return dbr_packlenf(MEMB_FORMAT,
                        memb->accnt.id, memb->trader.id);
}

DBR_API char*
dbr_writetrade(char* buf, const struct DbrTrade* trade)
{
    return dbr_packf(buf, TRADE_FORMAT,
                     trade->id, trade->match, trade->order, trade->order_rev,
                     trade->trader.id, trade->accnt.id, DBR_REF_MAX, trade->ref,
                     trade->market.id, trade->cpty.id, trade->role, trade->action,
                     trade->ticks, trade->resd, trade->exec, trade->lots,
                     trade->settl_date, trade->created, trade->modified);
}

DBR_API const char*
dbr_readtrade(const char* buf, struct DbrTrade* trade)
{
    return dbr_unpackf(buf, TRADE_FORMAT,
                       &trade->id, &trade->match, &trade->order, &trade->order_rev,
                       &trade->trader.id, &trade->accnt.id, DBR_REF_MAX, trade->ref,
                       &trade->market.id, &trade->cpty.id, &trade->role, &trade->action,
                       &trade->ticks, &trade->resd, &trade->exec, &trade->lots,
                       &trade->settl_date, &trade->created, &trade->modified);
}

DBR_API int
dbr_tradelen(const struct DbrTrade* trade)
{
    return dbr_packlenf(TRADE_FORMAT,
                        trade->id, trade->match, trade->order, trade->order_rev,
                        trade->trader.id, trade->accnt.id, DBR_REF_MAX, trade->ref,
                        trade->market.id, trade->cpty.id, trade->role, trade->action,
                        trade->ticks, trade->resd, trade->exec, trade->lots,
                        trade->settl_date, trade->created, trade->modified);
}

DBR_API char*
dbr_writeposn(char* buf, const struct DbrPosn* posn)
{
    return dbr_packf(buf, POSN_FORMAT,
                     posn->id, posn->accnt.id, posn->instr.id, posn->settl_date,
                     posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
}

DBR_API const char*
dbr_readposn(const char* buf, struct DbrPosn* posn)
{
    return dbr_unpackf(buf, POSN_FORMAT,
                       &posn->id, &posn->accnt.id, &posn->instr.id, &posn->settl_date,
                       &posn->buy_licks, &posn->buy_lots, &posn->sell_licks, &posn->sell_lots);
}

DBR_API int
dbr_posnlen(const char* buf, const struct DbrPosn* posn)
{
    return dbr_packlenf(buf, POSN_FORMAT,
                        posn->id, posn->accnt.id, posn->instr.id, posn->settl_date,
                        posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);
}
