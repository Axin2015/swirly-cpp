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
static const char TRADE_FORMAT[] = "llilllisillllllill";
static const char MEMB_FORMAT[] = "ll";
static const char POSN_FORMAT[] = "lllllll";

static const char INSERT_ORDER_FORMAT[] = "liilllisilllllll";
static const char UPDATE_ORDER_FORMAT[] = "liillll";
static const char ARCHIVE_ORDER_FORMAT[] = "ll";
static const char INSERT_TRADE_FORMAT[] = "llilllisillllllil";
static const char ARCHIVE_TRADE_FORMAT[] = "ll";

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
                         order->id, order->rev, order->status, order->trader.rec->id,
                         order->accnt.rec->id, order->contr.rec->id, order->settl_date,
                         DBR_REF_MAX, order->ref, order->action, order->ticks, order->resd,
                         order->exec, order->lots, order->min, order->flags, order->created,
                         order->modified);
    } else {
        n = dbr_packlenf(ORDER_FORMAT,
                         order->id, order->rev, order->status, order->trader.id_only,
                         order->accnt.id_only, order->contr.id_only, order->settl_date,
                         DBR_REF_MAX, order->ref, order->action, order->ticks, order->resd,
                         order->exec, order->lots, order->min, order->flags, order->created,
                         order->modified);
    }
    return n;
}

DBR_API char*
dbr_write_order(char* buf, const struct DbrOrder* order, DbrBool enriched)
{
    if (enriched) {
        buf = dbr_packf(buf, ORDER_FORMAT,
                        order->id, order->rev, order->status, order->trader.rec->id,
                        order->accnt.rec->id, order->contr.rec->id, order->settl_date, DBR_REF_MAX,
                        order->ref, order->action, order->ticks, order->resd, order->exec,
                        order->lots, order->min, order->flags, order->created, order->modified);
    } else {
        buf = dbr_packf(buf, ORDER_FORMAT,
                        order->id, order->rev, order->status, order->trader.id_only,
                        order->accnt.id_only, order->contr.id_only, order->settl_date, DBR_REF_MAX,
                        order->ref, order->action, order->ticks, order->resd, order->exec,
                        order->lots, order->min, order->flags, order->created, order->modified);
    }
    return buf;
}

DBR_API const char*
dbr_read_order(const char* buf, struct DbrOrder* order)
{
    dbr_order_init(order);
    return dbr_unpackf(buf, ORDER_FORMAT,
                       &order->id, &order->rev, &order->status, &order->trader.id_only,
                       &order->accnt.id_only, &order->contr.id_only, &order->settl_date,
                       DBR_REF_MAX, order->ref, &order->action, &order->ticks, &order->resd,
                       &order->exec, &order->lots, &order->min, &order->flags, &order->created,
                       &order->modified);
}

DBR_API size_t
dbr_trade_len(const struct DbrTrade* trade, DbrBool enriched)
{
    size_t n;
    if (enriched) {
        n = dbr_packlenf(TRADE_FORMAT,
                         trade->id, trade->order, trade->rev, trade->trader.rec->id,
                         trade->accnt.rec->id, trade->contr.rec->id, trade->settl_date,
                         DBR_REF_MAX, trade->ref, trade->action, trade->ticks, trade->resd,
                         trade->exec, trade->lots, trade->match, trade->cpty.rec->id,
                         trade->role, trade->created, trade->modified);
    } else {
        n = dbr_packlenf(TRADE_FORMAT,
                         trade->id, trade->order, trade->rev, trade->trader.id_only,
                         trade->accnt.id_only, trade->contr.id_only, trade->settl_date,
                         DBR_REF_MAX, trade->ref, trade->action, trade->ticks, trade->resd,
                         trade->exec, trade->lots, trade->match, trade->cpty.id_only,
                         trade->role, trade->created, trade->modified);
    }
    return n;
}

DBR_API char*
dbr_write_trade(char* buf, const struct DbrTrade* trade, DbrBool enriched)
{
    if (enriched) {
        buf = dbr_packf(buf, TRADE_FORMAT,
                        trade->id, trade->order, trade->rev, trade->trader.rec->id,
                        trade->accnt.rec->id, trade->contr.rec->id, trade->settl_date,
                        DBR_REF_MAX, trade->ref, trade->action, trade->ticks, trade->resd,
                        trade->exec, trade->lots, trade->match, trade->cpty.rec->id,
                        trade->role, trade->created, trade->modified);
    } else {
        buf = dbr_packf(buf, TRADE_FORMAT,
                        trade->id, trade->order, trade->rev, trade->trader.id_only,
                        trade->accnt.id_only, trade->contr.id_only, trade->settl_date,
                        DBR_REF_MAX, trade->ref, trade->action, trade->ticks, trade->resd,
                        trade->exec, trade->lots, trade->match, trade->cpty.id_only,
                        trade->role, trade->created, trade->modified);
    }
    return buf;
}

DBR_API const char*
dbr_read_trade(const char* buf, struct DbrTrade* trade)
{
    dbr_trade_init(trade);
    return dbr_unpackf(buf, TRADE_FORMAT,
                       &trade->id, &trade->order, &trade->rev, &trade->trader.id_only,
                       &trade->accnt.id_only, &trade->contr.id_only, &trade->settl_date,
                       DBR_REF_MAX, trade->ref, &trade->action, &trade->ticks, &trade->resd,
                       &trade->exec, &trade->lots, &trade->match, &trade->cpty.id_only,
                       &trade->role, &trade->created, &trade->modified);
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

DBR_API size_t
dbr_stmt_len(const struct DbrStmt* stmt)
{
    size_t n = dbr_packleni(stmt->type);
    switch (stmt->type) {
    case DBR_INSERT_ORDER:
        n += dbr_packlenf(INSERT_ORDER_FORMAT,
                          stmt->insert_order.id, stmt->insert_order.rev,
                          stmt->insert_order.status, stmt->insert_order.tid,
                          stmt->insert_order.aid, stmt->insert_order.cid,
                          stmt->insert_order.settl_date, DBR_REF_MAX,
                          stmt->insert_order.ref, stmt->insert_order.action,
                          stmt->insert_order.ticks, stmt->insert_order.resd,
                          stmt->insert_order.exec, stmt->insert_order.lots,
                          stmt->insert_order.min, stmt->insert_order.flags,
                          stmt->insert_order.now);
        break;
	case DBR_UPDATE_ORDER:
        n += dbr_packlenf(UPDATE_ORDER_FORMAT,
                          stmt->update_order.id, stmt->update_order.rev,
                          stmt->update_order.status, stmt->update_order.resd,
                          stmt->update_order.exec, stmt->update_order.lots,
                          stmt->update_order.now);
        break;
	case DBR_ARCHIVE_ORDER:
        n += dbr_packlenf(ARCHIVE_ORDER_FORMAT,
                          stmt->archive_order.id, stmt->archive_order.now);
        break;
	case DBR_INSERT_TRADE:
        n += dbr_packlenf(INSERT_TRADE_FORMAT,
                          stmt->insert_trade.id, stmt->insert_trade.order,
                          stmt->insert_trade.rev, stmt->insert_trade.tid,
                          stmt->insert_trade.aid, stmt->insert_trade.cid,
                          stmt->insert_trade.settl_date, DBR_REF_MAX,
                          stmt->insert_trade.ref, stmt->insert_trade.action,
                          stmt->insert_trade.ticks, stmt->insert_trade.resd,
                          stmt->insert_trade.exec, stmt->insert_trade.lots,
                          stmt->insert_trade.match, stmt->insert_trade.cpty,
                          stmt->insert_trade.role, stmt->insert_trade.now);
        break;
	case DBR_ARCHIVE_TRADE:
        n += dbr_packlenf(ARCHIVE_TRADE_FORMAT,
                          stmt->archive_trade.id, stmt->archive_trade.now);
        break;
    default:
        abort();
    }
    return n;
}

DBR_API char*
dbr_write_stmt(char* buf, const struct DbrStmt* stmt)
{
    buf = dbr_packi(buf, stmt->type);
    switch (stmt->type) {
    case DBR_INSERT_ORDER:
        buf = dbr_packf(buf, INSERT_ORDER_FORMAT,
                        stmt->insert_order.id, stmt->insert_order.rev,
                        stmt->insert_order.status, stmt->insert_order.tid,
                        stmt->insert_order.aid, stmt->insert_order.cid,
                        stmt->insert_order.settl_date, DBR_REF_MAX,
                        stmt->insert_order.ref, stmt->insert_order.action,
                        stmt->insert_order.ticks, stmt->insert_order.resd,
                        stmt->insert_order.exec, stmt->insert_order.lots,
                        stmt->insert_order.min, stmt->insert_order.flags,
                        stmt->insert_order.now);
        break;
	case DBR_UPDATE_ORDER:
        buf = dbr_packf(buf, UPDATE_ORDER_FORMAT,
                        stmt->update_order.id, stmt->update_order.rev,
                        stmt->update_order.status, stmt->update_order.resd,
                        stmt->update_order.exec, stmt->update_order.lots,
                        stmt->update_order.now);
        break;
	case DBR_ARCHIVE_ORDER:
        buf = dbr_packf(buf, ARCHIVE_ORDER_FORMAT,
                        stmt->archive_order.id, stmt->archive_order.now);
        break;
	case DBR_INSERT_TRADE:
        buf = dbr_packf(buf, INSERT_TRADE_FORMAT,
                        stmt->insert_trade.id, stmt->insert_trade.order,
                        stmt->insert_trade.rev, stmt->insert_trade.tid,
                        stmt->insert_trade.aid, stmt->insert_trade.cid,
                        stmt->insert_trade.settl_date, DBR_REF_MAX,
                        stmt->insert_trade.ref, stmt->insert_trade.action,
                        stmt->insert_trade.ticks, stmt->insert_trade.resd,
                        stmt->insert_trade.exec, stmt->insert_trade.lots,
                        stmt->insert_trade.match, stmt->insert_trade.cpty,
                        stmt->insert_trade.role, stmt->insert_trade.now);
        break;
	case DBR_ARCHIVE_TRADE:
        buf = dbr_packf(buf, ARCHIVE_TRADE_FORMAT,
                        stmt->archive_trade.id, stmt->archive_trade.now);
        break;
    default:
        abort();
    }
    return buf;
}

DBR_API const char*
dbr_read_stmt(const char* buf, struct DbrStmt* stmt)
{
    dbr_stmt_init(stmt);
    buf = dbr_unpacki(buf, &stmt->type);
    switch (stmt->type) {
    case DBR_INSERT_ORDER:
        buf = dbr_unpackf(buf, INSERT_ORDER_FORMAT,
                          &stmt->insert_order.id, &stmt->insert_order.rev,
                          &stmt->insert_order.status, &stmt->insert_order.tid,
                          &stmt->insert_order.aid, &stmt->insert_order.cid,
                          &stmt->insert_order.settl_date, DBR_REF_MAX,
                          &stmt->insert_order.ref, &stmt->insert_order.action,
                          &stmt->insert_order.ticks, &stmt->insert_order.resd,
                          &stmt->insert_order.exec, &stmt->insert_order.lots,
                          &stmt->insert_order.min, &stmt->insert_order.flags,
                          &stmt->insert_order.now);
        break;
	case DBR_UPDATE_ORDER:
        buf = dbr_unpackf(buf, UPDATE_ORDER_FORMAT,
                          &stmt->update_order.id, &stmt->update_order.rev,
                          &stmt->update_order.status, &stmt->update_order.resd,
                          &stmt->update_order.exec, &stmt->update_order.lots,
                          &stmt->update_order.now);
        break;
	case DBR_ARCHIVE_ORDER:
        buf = dbr_unpackf(buf, ARCHIVE_ORDER_FORMAT,
                          &stmt->archive_order.id, &stmt->archive_order.now);
        break;
	case DBR_INSERT_TRADE:
        buf = dbr_unpackf(buf, INSERT_TRADE_FORMAT,
                          &stmt->insert_trade.id, &stmt->insert_trade.order,
                          &stmt->insert_trade.rev, &stmt->insert_trade.tid,
                          &stmt->insert_trade.aid, &stmt->insert_trade.cid,
                          &stmt->insert_trade.settl_date, DBR_REF_MAX,
                          &stmt->insert_trade.ref, &stmt->insert_trade.action,
                          &stmt->insert_trade.ticks, &stmt->insert_trade.resd,
                          &stmt->insert_trade.exec, &stmt->insert_trade.lots,
                          &stmt->insert_trade.match, &stmt->insert_trade.cpty,
                          &stmt->insert_trade.role, &stmt->insert_trade.now);
        break;
	case DBR_ARCHIVE_TRADE:
        buf = dbr_unpackf(buf, ARCHIVE_TRADE_FORMAT,
                          &stmt->archive_trade.id, &stmt->archive_trade.now);
        break;
    default:
        dbr_err_setf(DBR_EIO, "invalid type %d", stmt->type);
        buf = NULL;
    }
    return buf;
}
