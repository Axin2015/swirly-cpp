/**
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
#define _XOPEN_SOURCE 700 // strnlen()
#include <dbr/string.h>

#include <dbr/conv.h>
#include <dbr/err.h>
#include <dbr/util.h>

#include <elm/market.h>
#include <elm/side.h>

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

static const char DIGITS[] = "0123456789";

static char*
printi(char* buf, int i)
{
    if (i == 0) {
        *buf++ = '0';
        goto done;
    }

    unsigned int ui;
    if (i < 0) {
        ui = -i;
        *buf++ = '-';
    } else
        ui = i;

    char* cp = (buf += dbr_intdig(i));
    do {
        *--cp = DIGITS[ui % 10];
    } while ((ui /= 10));
 done:
    return buf;
}

static char*
printl(char* buf, long l)
{
    if (l == 0) {
        *buf++ = '0';
        goto done;
    }

    unsigned long ul;
    if (l < 0) {
        ul = -l;
        *buf++ = '-';
    } else
        ul = l;

    char* cp = (buf += dbr_longdig(l));
    do {
        *--cp = DIGITS[ul % 10];
    } while ((ul /= 10));
 done:
    return buf;
}

static const char*
action(int action)
{
    const char* sym;
    switch (action) {
    case DBR_BUY:
        sym = "BUY";
        break;
    case DBR_SELL:
        sym = "SELL";
        break;
    default:
        sym = "";
        break;
    }
    return sym;
}

static size_t
actionlen(int action)
{
    size_t len;
    switch (action) {
    case DBR_BUY:
        len = sizeof("BUY") - 1;
        break;
    case DBR_SELL:
        len = sizeof("SELL") - 1;
        break;
    default:
        len = 0;
        break;
    }
    return len;
}

static const char*
role(int role)
{
    const char* sym;
    switch (role) {
    case DBR_MAKER:
        sym = "MAKER";
        break;
    case DBR_TAKER:
        sym = "TAKER";
        break;
    default:
        sym = "";
    }
    return sym;
}

static size_t
rolelen(int role)
{
    size_t len;
    switch (role) {
    case DBR_MAKER:
        len = sizeof("MAKER") - 1;
        break;
    case DBR_TAKER:
        len = sizeof("TAKER") - 1;
        break;
    default:
        len = 0;
    }
    return len;
}

static const char*
status(int status)
{
    const char* sym;
    switch (status) {
    case DBR_NEW:
        sym = "NEW";
        break;
    case DBR_REVISED:
        sym = "REVISED";
        break;
    case DBR_CANCELLED:
        sym = "CANCELLED";
        break;
    case DBR_REJECTED:
        sym = "REJECTED";
        break;
    case DBR_PARTIAL:
        sym = "PARTIAL";
        break;
    case DBR_FILLED:
        sym = "FILLED";
        break;
    default:
        sym = "";
        break;
    }
    return sym;
}

static size_t
statuslen(int status)
{
    size_t len;
    switch (status) {
    case DBR_NEW:
        len = sizeof("NEW") - 1;
        break;
    case DBR_REVISED:
        len = sizeof("REVISED") - 1;
        break;
    case DBR_CANCELLED:
        len = sizeof("CANCELLED") - 1;
        break;
    case DBR_REJECTED:
        len = sizeof("REJECTED") - 1;
        break;
    case DBR_PARTIAL:
        len = sizeof("PARTIAL") - 1;
        break;
    case DBR_FILLED:
        len = sizeof("FILLED") - 1;
        break;
    default:
        len = 0;
        break;
    }
    return len;
}

DBR_API void
dbr_format_price(double price, int price_dp, int pip_dp, struct DbrPriceString* ps)
{
    static const char DIGITS[] = "0123456789";

    // Digits in price as integer.
    int digits = fabs(price) * pow(10, price_dp) + 0.5;
    char* cp = ps->buf + sizeof(ps->buf) - 1;

    *cp-- = '\0';
    while (price_dp > pip_dp) {
        *cp-- = DIGITS[digits % 10];
        digits /= 10;
        if (--price_dp == 0)
            *cp-- = '.';
    }
    ps->small = cp + 1;

    // Lower-order pip digit.
    *cp-- = '\0';
    *cp-- = DIGITS[digits % 10];
    digits /= 10;

    if (--price_dp == 0)
        *cp-- = '.';

    // Higher-order pip digit.
    *cp-- = DIGITS[digits % 10];
    digits /= 10;
    ps->pips = cp + 1;

    *cp-- = '\0';
    while (digits > 0 || price_dp > 0) {
        if (--price_dp == 0)
            *cp-- = '.';
        *cp-- = DIGITS[digits % 10];
        digits /= 10;
    }
    // Signedness.
    if (price < 0)
        *cp-- = '-';

    ps->big = cp + 1;
}

DBR_API char*
dbr_vformat(char* buf, const char* format, va_list args)
{
    assert(buf);
    assert(format);

    int esc = 0;
    for (const char* cp = format; *cp != '\0'; ++cp) {
        const char ch = *cp;
        if (esc) {
            int i;
            long l;
            const char* s;
            switch (ch) {
            case 'A':
                i = va_arg(args, int);
                buf = stpcpy(buf, action(i));
                break;
            case 'R':
                i = va_arg(args, int);
                buf = stpcpy(buf, role(i));
                break;
            case 'S':
                i = va_arg(args, int);
                buf = stpcpy(buf, status(i));
                break;
            case 'd':
            case 'i':
                i = va_arg(args, int);
                buf = printi(buf, i);
                break;
            case 'l':
                l = va_arg(args, long);
                buf = printl(buf, l);
                break;
            case 'm':
                i = DBR_MNEM_MAX;
                s = va_arg(args, const char*);
                // stpncpy() zero pads so cannot use.
                for (; *s != '\0' && i > 0; ++s, --i)
                    *buf++ = *s;
                break;
            case 's':
                i = va_arg(args, int);
                s = va_arg(args, const char*);
                // stpncpy() zero pads so cannot use.
                for (; *s != '\0' && i > 0; ++s, --i)
                    *buf++ = *s;
                break;
            default:
                *buf++ = ch;
                break;
            }
            esc = 0;
        } else if (ch == '%')
            esc = 1;
        else
            *buf++ = ch;
    }
    return buf;
}

DBR_API char*
dbr_format(char* buf, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    buf = dbr_vformat(buf, format, args);
    va_end(args);
    return buf;
}

DBR_API char*
dbr_instrcpy(char* buf, const struct DbrRec* rec)
{
    assert(rec->type == DBR_INSTR);
    static const char INSTR_FORMAT[] =
        "{\"mnem\":\"%m\","
        "\"display\":\"%s\","
        "\"asset_type\":\"%m\","
        "\"instr_type\":\"%m\","
        "\"asset\":\"%m\","
        "\"ccy\":\"%m\","
        "\"tick_numer\":%d,"
        "\"tick_denom\":%d,"
        "\"lot_numer\":%d,"
        "\"lot_denom\":%d,"
        "\"price_dp\":%d,"
        "\"pip_dp\":%d,"
        "\"qty_dp\":%d,"
        "\"min_lots\":%l,"
        "\"max_lots\":%l}";

    return dbr_format(buf, INSTR_FORMAT,
                      rec->mnem,
                      DBR_DISPLAY_MAX, rec->instr.display,
                      rec->instr.asset_type,
                      rec->instr.instr_type,
                      rec->instr.asset,
                      rec->instr.ccy,
                      rec->instr.tick_numer,
                      rec->instr.tick_denom,
                      rec->instr.lot_numer,
                      rec->instr.lot_denom,
                      rec->instr.price_dp,
                      rec->instr.pip_dp,
                      rec->instr.qty_dp,
                      rec->instr.min_lots,
                      rec->instr.max_lots);
}

DBR_API size_t
dbr_instrlen(const struct DbrRec* rec)
{
    assert(rec->type == DBR_INSTR);
    enum {
        INSTR_SIZE =
        sizeof("{\"mnem\":\"\","
               "\"display\":\"\","
               "\"asset_type\":\"\","
               "\"instr_type\":\"\","
               "\"asset\":\"\","
               "\"ccy\":\"\","
               "\"tick_numer\":,"
               "\"tick_denom\":,"
               "\"lot_numer\":,"
               "\"lot_denom\":,"
               "\"price_dp\":,"
               "\"pip_dp\":,"
               "\"qty_dp\":,"
               "\"min_lots\":,"
               "\"max_lots\":}") - 1
    };

    return INSTR_SIZE
        + strnlen(rec->mnem, DBR_MNEM_MAX)
        + strnlen(rec->instr.display, DBR_DISPLAY_MAX)
        + strnlen(rec->instr.asset_type, DBR_MNEM_MAX)
        + strnlen(rec->instr.instr_type, DBR_MNEM_MAX)
        + strnlen(rec->instr.asset, DBR_MNEM_MAX)
        + strnlen(rec->instr.ccy, DBR_MNEM_MAX)
        + dbr_intlen(rec->instr.tick_numer)
        + dbr_intlen(rec->instr.tick_denom)
        + dbr_intlen(rec->instr.lot_numer)
        + dbr_intlen(rec->instr.lot_denom)
        + dbr_intlen(rec->instr.price_dp)
        + dbr_intlen(rec->instr.pip_dp)
        + dbr_intlen(rec->instr.qty_dp)
        + dbr_longlen(rec->instr.min_lots)
        + dbr_longlen(rec->instr.max_lots);
}

DBR_API char*
dbr_marketcpy(char* buf, struct DbrRec* rec)
{
    assert(rec->type == DBR_MARKET);
    static const char MARKET_FORMAT[] =
        "{\"mnem\":\"%m\","
        "\"instr\":\"%m\","
        "\"tenor\":\"%s\","
        "\"bid_ticks\":%l,"
        "\"bid_resd\":%l,"
        "\"ask_ticks\":%l,"
        "\"ask_resd\":%l}";

    struct DbrBest best;
    elm_market_best(rec, &best);
    return dbr_format(buf, MARKET_FORMAT,
                      rec->mnem,
                      rec->market.instr.rec->mnem,
                      DBR_TENOR_MAX, rec->market.tenor,
                      best.bid_ticks,
                      best.bid_resd,
                      best.ask_ticks,
                      best.ask_resd);
}

DBR_API size_t
dbr_marketlen(struct DbrRec* rec)
{
    assert(rec->type == DBR_MARKET);
    enum {
        MARKET_SIZE =
        sizeof("{\"mnem\":\"\","
               "\"instr\":\"\","
               "\"tenor\":\"\","
               "\"bid_ticks\":,"
               "\"bid_resd\":,"
               "\"ask_ticks\":,"
               "\"ask_resd\":}") - 1
    };

    struct DbrBest best;
    elm_market_best(rec, &best);
    return MARKET_SIZE
        + strnlen(rec->mnem, DBR_MNEM_MAX)
        + strnlen(rec->market.instr.rec->mnem, DBR_MNEM_MAX)
        + strnlen(rec->market.tenor, DBR_TENOR_MAX)
        + dbr_longlen(best.bid_ticks)
        + dbr_longlen(best.bid_resd)
        + dbr_longlen(best.ask_ticks)
        + dbr_longlen(best.ask_resd);
}

DBR_API char*
dbr_tradercpy(char* buf, const struct DbrRec* rec)
{
    assert(rec->type == DBR_TRADER);
    static const char TRADER_FORMAT[] =
        "{\"mnem\":\"%m\","
        "\"display\":\"%s\","
        "\"email\":\"%s\"}";

    return dbr_format(buf, TRADER_FORMAT,
                      rec->mnem,
                      DBR_DISPLAY_MAX, rec->trader.display,
                      DBR_EMAIL_MAX, rec->trader.email);
}

DBR_API size_t
dbr_traderlen(const struct DbrRec* rec)
{
    assert(rec->type == DBR_TRADER);
    enum {
        TRADER_SIZE =
        sizeof("{\"mnem\":\"\","
               "\"display\":\"\","
               "\"email\":\"\"}") - 1
    };

    return TRADER_SIZE
        + strnlen(rec->mnem, DBR_MNEM_MAX)
        + strnlen(rec->trader.display, DBR_DISPLAY_MAX)
        + strnlen(rec->trader.email, DBR_EMAIL_MAX);
}

DBR_API char*
dbr_accntcpy(char* buf, const struct DbrRec* rec)
{
    assert(rec->type == DBR_ACCNT);
    static const char ACCNT_FORMAT[] =
        "{\"mnem\":\"%m\","
        "\"display\":\"%s\","
        "\"email\":\"%s\"}";

    return dbr_format(buf, ACCNT_FORMAT,
                      rec->mnem,
                      DBR_DISPLAY_MAX, rec->accnt.display,
                      DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_API size_t
dbr_accntlen(const struct DbrRec* rec)
{
    assert(rec->type == DBR_ACCNT);
    enum {
        ACCNT_SIZE =
        sizeof("{\"mnem\":\"\","
               "\"display\":\"\","
               "\"email\":\"\"}") - 1
    };

    return ACCNT_SIZE
        + strnlen(rec->mnem, DBR_MNEM_MAX)
        + strnlen(rec->accnt.display, DBR_DISPLAY_MAX)
        + strnlen(rec->accnt.email, DBR_EMAIL_MAX);
}

DBR_API char*
dbr_sidecpy(char* buf, DbrSide side)
{
    enum { LEVELS = 5 };
    static const char SIDE_FORMAT[] =
        "{\"count\":[%l,%l,%l,%l,%l],"
        "\"ticks\":[%l,%l,%l,%l,%l],"
        "\"resd\":[%l,%l,%l,%l,%l]}";

    size_t count[LEVELS];
    DbrTicks ticks[LEVELS];
    DbrLots resd[LEVELS];

    struct DbrRbNode* it = elm_side_first_level(side);
    struct DbrRbNode* end = elm_side_end_level(side);
    for (size_t i = 0; i < LEVELS; ++i) {
        if (it != end) {
            struct DbrLevel* level = dbr_side_level_entry(it);
            count[i] = level->count;
            ticks[i] = level->ticks;
            resd[i] = level->resd;
            it = dbr_rbnode_next(it);
        } else {
            count[i] = 0;
            ticks[i] = 0;
            resd[i] = 0;
        }
    }

    return dbr_format(buf, SIDE_FORMAT,
                      count[0], count[1], count[2], count[3], count[4],
                      ticks[0], ticks[1], ticks[2], ticks[3], ticks[4],
                       resd[0],  resd[1],  resd[2],  resd[3],  resd[4]);
}

DBR_API size_t
dbr_sidelen(DbrSide side)
{
    enum { LEVELS = 5 };
    enum {
        SIDE_SIZE =
        sizeof("{\"count\":[,,,,],"
               "\"ticks\":[,,,,],"
               "\"resd\":[,,,,]}") - 1
    };

    size_t len = SIDE_SIZE;
    struct DbrRbNode* it = elm_side_first_level(side);
    struct DbrRbNode* end = elm_side_end_level(side);
    for (size_t i = 0; i < LEVELS; ++i) {
        if (it != end) {
            struct DbrLevel* level = dbr_side_level_entry(it);
            len += dbr_longlen(level->count)
                + dbr_longlen(level->ticks)
                + dbr_longlen(level->resd);
            it = dbr_rbnode_next(it);
        } else {
            // Zero for each array.
            len += sizeof("000") - 1;
        }
    }
    return len;
}

DBR_API char*
dbr_ordercpy(char* buf, const struct DbrOrder* order)
{
    static const char ORDER_FORMAT[] =
        "{\"id\":%l,"
        "\"rev\":%d,"
        "\"status\":\"%S\","
        "\"trader\":\"%m\","
        "\"accnt\":\"%m\","
        "\"ref\":\"%s\","
        "\"market\":\"%m\","
        "\"action\":\"%A\","
        "\"ticks\":%l,"
        "\"resd\":%l,"
        "\"exec\":%l,"
        "\"lots\":%l,"
        "\"min\":%l,"
        "\"flags\":%l,"
        "\"created\":%l,"
        "\"modified\":%l}";

    return dbr_format(buf, ORDER_FORMAT,
                      order->id,
                      order->rev,
                      order->status,
                      order->trader.rec->mnem,
                      order->accnt.rec->mnem,
                      DBR_REF_MAX, order->ref,
                      order->market.rec->mnem,
                      order->action,
                      order->ticks,
                      order->resd,
                      order->exec,
                      order->lots,
                      order->min,
                      order->flags,
                      order->created,
                      order->modified);
}

DBR_API size_t
dbr_orderlen(const struct DbrOrder* order)
{
    enum {
        ORDER_SIZE =
        sizeof("{\"id\":,"
               "\"rev\":,"
               "\"status\":\"\","
               "\"trader\":\"\","
               "\"accnt\":\"\","
               "\"ref\":\"\","
               "\"market\":\"\","
               "\"action\":\"\","
               "\"ticks\":,"
               "\"resd\":,"
               "\"exec\":,"
               "\"lots\":,"
               "\"min\":,"
               "\"flags\":,"
               "\"created\":,"
               "\"modified\":}") - 1
    };

    return ORDER_SIZE
        + dbr_longlen(order->id)
        + dbr_intlen(order->rev)
        + statuslen(order->status)
        + strnlen(order->trader.rec->mnem, DBR_MNEM_MAX)
        + strnlen(order->accnt.rec->mnem, DBR_MNEM_MAX)
        + strnlen(order->ref, DBR_REF_MAX)
        + strnlen(order->market.rec->mnem, DBR_MNEM_MAX)
        + actionlen(order->action)
        + dbr_longlen(order->ticks)
        + dbr_longlen(order->resd)
        + dbr_longlen(order->exec)
        + dbr_longlen(order->lots)
        + dbr_longlen(order->min)
        + dbr_longlen(order->flags)
        + dbr_longlen(order->created)
        + dbr_longlen(order->modified);
}

DBR_API char*
dbr_tradecpy(char* buf, const struct DbrTrade* trade)
{
    static const char TRADE_FORMAT[] =
        "{\"id\":%l,"
        "\"match\":%l,"
        "\"order\":%l,"
        "\"order_rev\":%d,"
        "\"trader\":\"%m\","
        "\"accnt\":\"%m\","
        "\"ref\":\"%s\","
        "\"market\":\"%m\","
        "\"cpty\":\"%m\","
        "\"role\":\"%R\","
        "\"action\":\"%A\","
        "\"ticks\":%l,"
        "\"resd\":%l,"
        "\"exec\":%l,"
        "\"lots\":%l,"
        "\"settl_date\":%d,"
        "\"created\":%l,"
        "\"modified\":%l}";

    return dbr_format(buf, TRADE_FORMAT,
                      trade->id,
                      trade->match,
                      trade->order,
                      trade->order_rev,
                      trade->trader.rec->mnem,
                      trade->accnt.rec->mnem,
                      DBR_REF_MAX, trade->ref,
                      trade->market.rec->mnem,
                      trade->cpty.rec->mnem,
                      trade->role,
                      trade->action,
                      trade->ticks,
                      trade->resd,
                      trade->exec,
                      trade->lots,
                      trade->settl_date,
                      trade->created,
                      trade->modified);
}

DBR_API size_t
dbr_tradelen(const struct DbrTrade* trade)
{
    enum {
        TRADE_SIZE =
        sizeof("{\"id\":,"
               "\"match\":,"
               "\"order\":,"
               "\"order_rev\":,"
               "\"trader\":\"\","
               "\"accnt\":\"\","
               "\"ref\":\"\","
               "\"market\":\"\","
               "\"cpty\":\"\","
               "\"role\":\"\","
               "\"action\":\"\","
               "\"ticks\":,"
               "\"resd\":,"
               "\"exec\":,"
               "\"lots\":,"
               "\"settl_date\":,"
               "\"created\":,"
               "\"modified\":}") - 1
    };

    return TRADE_SIZE
        + dbr_longlen(trade->id)
        + dbr_longlen(trade->match)
        + dbr_longlen(trade->order)
        + dbr_intlen(trade->order_rev)
        + strnlen(trade->trader.rec->mnem, DBR_MNEM_MAX)
        + strnlen(trade->accnt.rec->mnem, DBR_MNEM_MAX)
        + strnlen(trade->ref, DBR_REF_MAX)
        + strnlen(trade->market.rec->mnem, DBR_MNEM_MAX)
        + strnlen(trade->cpty.rec->mnem, DBR_MNEM_MAX)
        + rolelen(trade->role)
        + actionlen(trade->action)
        + dbr_longlen(trade->ticks)
        + dbr_longlen(trade->resd)
        + dbr_longlen(trade->exec)
        + dbr_longlen(trade->lots)
        + dbr_intlen(trade->settl_date)
        + dbr_longlen(trade->created)
        + dbr_longlen(trade->modified);
}

DBR_API char*
dbr_posncpy(char* buf, const struct DbrPosn* posn)
{
    static const char POSN_FORMAT[] =
        "{\"accnt\":\"%m\","
        "\"instr\":\"%m\","
        "\"settl_date\":%d,"
        "\"buy_licks\":%l,"
        "\"buy_lots\":%l,"
        "\"sell_licks\":%l,"
        "\"sell_lots\":%l}";

    return dbr_format(buf, POSN_FORMAT,
                      posn->accnt.rec->mnem,
                      posn->instr.rec->mnem,
                      posn->settl_date,
                      posn->buy_licks,
                      posn->buy_lots,
                      posn->sell_licks,
                      posn->sell_lots);
}

DBR_API size_t
dbr_posnlen(const struct DbrPosn* posn)
{
    enum {
        POSN_SIZE =
        sizeof("{\"accnt\":\"\","
               "\"instr\":\"\","
               "\"settl_date\":,"
               "\"buy_licks\":,"
               "\"buy_lots\":,"
               "\"sell_licks\":,"
               "\"sell_lots\":}") - 1
    };

    return POSN_SIZE
        + strnlen(posn->accnt.rec->mnem, DBR_MNEM_MAX)
        + strnlen(posn->instr.rec->mnem, DBR_MNEM_MAX)
        + dbr_intlen(posn->settl_date)
        + dbr_longlen(posn->buy_licks)
        + dbr_longlen(posn->buy_lots)
        + dbr_longlen(posn->sell_licks)
        + dbr_longlen(posn->sell_lots);
}

DBR_API char*
dbr_errcpy(char* buf, const struct DbrErr* err)
{
    static const char ERR_FORMAT[] =
        "{\"num\":\"%d\","
        "\"msg\":\"%s\"}";

    return dbr_format(buf, ERR_FORMAT,
                      err->num,
                      DBR_ERROR_MAX, err->msg);
}

DBR_API size_t
dbr_errlen(const struct DbrErr* err)
{
    enum {
        ERR_SIZE =
        sizeof("{\"num\":,"
               "\"msg\":\"\"}") - 1
    };

    return ERR_SIZE
        + dbr_intlen(err->num)
        + strnlen(err->msg, DBR_ERROR_MAX);
}
