/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#include <dbr/elm/json.h>

#include "format.h"

#include <dbr/elm/types.h>

#include <dbr/ash/util.h>

#include <assert.h>
#include <string.h>

DBR_API size_t
dbr_json_accnt_len(const struct DbrRec* rec)
{
    assert(rec->type == DBR_ENTITY_ACCNT);
    enum {
        ACCNT_SIZE =
        sizeof("{\"mnem\":\"\","
               "\"display\":\"\","
               "\"email\":\"\"}") - 1
    };

    return ACCNT_SIZE
        + strnlen(rec->mnem, DBR_MNEM_MAX)
        + strnlen(rec->display, DBR_DISPLAY_MAX)
        + strnlen(rec->accnt.email, DBR_EMAIL_MAX);
}

DBR_API char*
dbr_json_write_accnt(char* buf, const struct DbrRec* rec)
{
    assert(rec->type == DBR_ENTITY_ACCNT);
    static const char ACCNT_FORMAT[] =
        "{\"mnem\":\"%m\","
        "\"display\":\"%s\","
        "\"email\":\"%s\"}";

    return dbr_format(buf, ACCNT_FORMAT,
                      rec->mnem,
                      DBR_DISPLAY_MAX, rec->display,
                      DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_API size_t
dbr_json_contr_len(const struct DbrRec* rec)
{
    assert(rec->type == DBR_ENTITY_CONTR);
    enum {
        CONTR_SIZE =
        sizeof("{\"mnem\":\"\","
               "\"display\":\"\","
               "\"asset_type\":\"\","
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

    return CONTR_SIZE
        + strnlen(rec->mnem, DBR_MNEM_MAX)
        + strnlen(rec->display, DBR_DISPLAY_MAX)
        + strnlen(rec->contr.asset_type, DBR_MNEM_MAX)
        + strnlen(rec->contr.asset, DBR_MNEM_MAX)
        + strnlen(rec->contr.ccy, DBR_MNEM_MAX)
        + dbr_int_len(rec->contr.tick_numer)
        + dbr_int_len(rec->contr.tick_denom)
        + dbr_int_len(rec->contr.lot_numer)
        + dbr_int_len(rec->contr.lot_denom)
        + dbr_int_len(rec->contr.price_dp)
        + dbr_int_len(rec->contr.pip_dp)
        + dbr_int_len(rec->contr.qty_dp)
        + dbr_long_len(rec->contr.min_lots)
        + dbr_long_len(rec->contr.max_lots);
}

DBR_API char*
dbr_json_write_contr(char* buf, const struct DbrRec* rec)
{
    assert(rec->type == DBR_ENTITY_CONTR);
    static const char CONTR_FORMAT[] =
        "{\"mnem\":\"%m\","
        "\"display\":\"%s\","
        "\"asset_type\":\"%m\","
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

    return dbr_format(buf, CONTR_FORMAT,
                      rec->mnem,
                      DBR_DISPLAY_MAX, rec->display,
                      rec->contr.asset_type,
                      rec->contr.asset,
                      rec->contr.ccy,
                      rec->contr.tick_numer,
                      rec->contr.tick_denom,
                      rec->contr.lot_numer,
                      rec->contr.lot_denom,
                      rec->contr.price_dp,
                      rec->contr.pip_dp,
                      rec->contr.qty_dp,
                      rec->contr.min_lots,
                      rec->contr.max_lots);
}

DBR_API size_t
dbr_json_perm_len(const struct DbrPerm* perm)
{
    enum {
        PERM_SIZE =
        sizeof("{\"trader\":\"\","
               "\"giveup\":\"\"}") - 1
    };

    return PERM_SIZE
        + strnlen(perm->trader.rec->mnem, DBR_MNEM_MAX)
        + strnlen(perm->giveup.rec->mnem, DBR_MNEM_MAX);
}

DBR_API char*
dbr_json_write_perm(char* buf, const struct DbrPerm* perm)
{
    static const char PERM_FORMAT[] =
        "{\"trader\":\"%m\","
        "\"giveup\":\"%m\"}";

    return dbr_format(buf, PERM_FORMAT,
                      perm->trader.rec->mnem,
                      perm->giveup.rec->mnem);
}

DBR_API size_t
dbr_json_order_len(const struct DbrOrder* order)
{
    enum {
        ORDER_SIZE =
        sizeof("{\"id\":,"
               "\"trader\":\"\","
               "\"giveup\":\"\","
               "\"contr\":\"\","
               "\"settl_date\":,"
               "\"ref\":\"\","
               "\"state\":\"\","
               "\"action\":\"\","
               "\"ticks\":,"
               "\"lots\":,"
               "\"resd\":,"
               "\"exec\":,"
               "\"last_ticks\":,"
               "\"last_lots\":,"
               "\"created\":,"
               "\"modified\":}") - 1
    };

    return ORDER_SIZE
        + dbr_long_len(order->id)
        + strnlen(order->i.trader.rec->mnem, DBR_MNEM_MAX)
        + strnlen(order->i.giveup.rec->mnem, DBR_MNEM_MAX)
        + strnlen(order->i.contr.rec->mnem, DBR_MNEM_MAX)
        + 8
        + strnlen(order->i.ref, DBR_REF_MAX)
        + elm_state_len(order->i.state)
        + elm_action_len(order->i.action)
        + dbr_long_len(order->i.ticks)
        + dbr_long_len(order->i.lots)
        + dbr_long_len(order->i.resd)
        + dbr_long_len(order->i.exec)
        + dbr_long_len(order->i.last_ticks)
        + dbr_long_len(order->i.last_lots)
        + dbr_long_len(order->created)
        + dbr_long_len(order->modified);
}

DBR_API char*
dbr_json_write_order(char* buf, const struct DbrOrder* order)
{
    static const char ORDER_FORMAT[] =
        "{\"id\":%l,"
        "\"trader\":\"%m\","
        "\"giveup\":\"%m\","
        "\"contr\":\"%m\","
        "\"settl_date\":%j,"
        "\"ref\":\"%s\","
        "\"state\":\"%S\","
        "\"action\":\"%A\","
        "\"ticks\":%l,"
        "\"lots\":%l,"
        "\"resd\":%l,"
        "\"exec\":%l,"
        "\"last_ticks\":%l,"
        "\"last_lots\":%l,"
        "\"created\":%l,"
        "\"modified\":%l}";

    return dbr_format(buf, ORDER_FORMAT,
                      order->id,
                      order->i.trader.rec->mnem,
                      order->i.giveup.rec->mnem,
                      order->i.contr.rec->mnem,
                      order->i.settl_day,
                      DBR_REF_MAX, order->i.ref,
                      order->i.state,
                      order->i.action,
                      order->i.ticks,
                      order->i.lots,
                      order->i.resd,
                      order->i.exec,
                      order->i.last_ticks,
                      order->i.last_lots,
                      order->created,
                      order->modified);
}

DBR_API size_t
dbr_json_exec_len(const struct DbrExec* exec)
{
    enum {
        EXEC_SIZE =
        sizeof("{\"id\":,"
               "\"order\":,"
               "\"trader\":\"\","
               "\"giveup\":\"\","
               "\"contr\":\"\","
               "\"settl_date\":,"
               "\"ref\":\"\","
               "\"state\":\"\","
               "\"action\":\"\","
               "\"ticks\":,"
               "\"lots\":,"
               "\"resd\":,"
               "\"exec\":,"
               "\"last_ticks\":,"
               "\"last_lots\":,"
               "\"match\":,"
               "\"role\":\"\","
               "\"cpty\":\"\","
               "\"created\":}") - 1
    };
    return EXEC_SIZE
        + dbr_long_len(exec->id)
        + dbr_long_len(exec->order)
        + strnlen(exec->i.trader.rec->mnem, DBR_MNEM_MAX)
        + strnlen(exec->i.giveup.rec->mnem, DBR_MNEM_MAX)
        + strnlen(exec->i.contr.rec->mnem, DBR_MNEM_MAX)
        + 8
        + strnlen(exec->i.ref, DBR_REF_MAX)
        + elm_state_len(exec->i.state)
        + elm_action_len(exec->i.action)
        + dbr_long_len(exec->i.ticks)
        + dbr_long_len(exec->i.lots)
        + dbr_long_len(exec->i.resd)
        + dbr_long_len(exec->i.exec)
        + dbr_long_len(exec->i.last_ticks)
        + dbr_long_len(exec->i.last_lots)
        + dbr_long_len(exec->match)
        + elm_role_len(exec->role)
        + strnlen(exec->cpty.rec->mnem, DBR_MNEM_MAX)
        + dbr_long_len(exec->created);
}

DBR_API char*
dbr_json_write_exec(char* buf, const struct DbrExec* exec)
{
    static const char EXEC_FORMAT[] =
        "{\"id\":%l,"
        "\"order\":%l,"
        "\"trader\":\"%m\","
        "\"giveup\":\"%m\","
        "\"contr\":\"%m\","
        "\"settl_date\":%j,"
        "\"ref\":\"%s\","
        "\"state\":\"%S\","
        "\"action\":\"%A\","
        "\"ticks\":%l,"
        "\"lots\":%l,"
        "\"resd\":%l,"
        "\"exec\":%l,"
        "\"last_ticks\":%l,"
        "\"last_lots\":%l,"
        "\"match\":%l,"
        "\"role\":\"%R\","
        "\"cpty\":\"%m\","
        "\"created\":%l}";

    return dbr_format(buf, EXEC_FORMAT,
                      exec->id,
                      exec->order,
                      exec->i.trader.rec->mnem,
                      exec->i.giveup.rec->mnem,
                      exec->i.contr.rec->mnem,
                      exec->i.settl_day,
                      DBR_REF_MAX, exec->i.ref,
                      exec->i.state,
                      exec->i.action,
                      exec->i.ticks,
                      exec->i.lots,
                      exec->i.resd,
                      exec->i.exec,
                      exec->i.last_ticks,
                      exec->i.last_lots,
                      exec->match,
                      exec->role,
                      exec->cpty.rec->mnem,
                      exec->created);
}

DBR_API size_t
dbr_json_posn_len(const struct DbrPosn* posn)
{
    enum {
        POSN_SIZE =
        sizeof("{\"accnt\":\"\","
               "\"contr\":\"\","
               "\"settl_date\":,"
               "\"buy_licks\":,"
               "\"buy_lots\":,"
               "\"sell_licks\":,"
               "\"sell_lots\":}") - 1
    };

    return POSN_SIZE
        + strnlen(posn->accnt.rec->mnem, DBR_MNEM_MAX)
        + strnlen(posn->contr.rec->mnem, DBR_MNEM_MAX)
        + 8
        + dbr_long_len(posn->buy_licks)
        + dbr_long_len(posn->buy_lots)
        + dbr_long_len(posn->sell_licks)
        + dbr_long_len(posn->sell_lots);
}

DBR_API char*
dbr_json_write_posn(char* buf, const struct DbrPosn* posn)
{
    static const char POSN_FORMAT[] =
        "{\"accnt\":\"%m\","
        "\"contr\":\"%m\","
        "\"settl_date\":%j,"
        "\"buy_licks\":%l,"
        "\"buy_lots\":%l,"
        "\"sell_licks\":%l,"
        "\"sell_lots\":%l}";

    return dbr_format(buf, POSN_FORMAT,
                      posn->accnt.rec->mnem,
                      posn->contr.rec->mnem,
                      posn->settl_day,
                      posn->buy_licks,
                      posn->buy_lots,
                      posn->sell_licks,
                      posn->sell_lots);
}

DBR_API size_t
dbr_json_view_len(const struct DbrView* view)
{
    enum { LEVELS = 3 };
    enum {
        SIDE_SIZE =
        sizeof("{\"contr\":\"\","
               "\"settl_date\":,"
               "\"bid_ticks\":[,,],"
               "\"bid_lots\":[,,],"
               "\"bid_count\":[,,],"
               "\"offer_ticks\":[,,],"
               "\"offer_lots\":[,,],"
               "\"offer_count\":[,,],"
               "\"created\":}") - 1
    };

    size_t len = SIDE_SIZE
        + strnlen(view->contr.rec->mnem, DBR_MNEM_MAX)
        + 8
        + dbr_long_len(view->created);

    for (size_t i = 0; i < DBR_LEVEL_MAX; ++i) {
        len += dbr_long_len(view->bid_ticks[i]);
        len += dbr_long_len(view->bid_lots[i]);
        len += dbr_long_len(view->bid_count[i]);
        len += dbr_long_len(view->offer_ticks[i]);
        len += dbr_long_len(view->offer_lots[i]);
        len += dbr_long_len(view->offer_count[i]);
    }
    return len;
}

DBR_API char*
dbr_json_write_view(char* buf, const struct DbrView* view)
{
    static const char VIEW_FORMAT[] =
        "{\"contr\":\"%m\","
        "\"settl_date\":%j,"
        "\"bid_ticks\":[%l,%l,%l],"
        "\"bid_lots\":[%l,%l,%l],"
        "\"bid_count\":[%l,%l,%l],"
        "\"offer_ticks\":[%l,%l,%l],"
        "\"offer_lots\":[%l,%l,%l],"
        "\"offer_count\":[%l,%l,%l],"
        "\"created\":%l}";

    return dbr_format(buf, VIEW_FORMAT,
                      view->contr.rec->mnem,
                      view->settl_day,
                      view->bid_ticks[0], view->bid_ticks[1], view->bid_ticks[2],
                      view->bid_lots[0], view->bid_lots[1], view->bid_lots[2],
                      view->bid_count[0], view->bid_count[1], view->bid_count[2],
                      view->offer_ticks[0], view->offer_ticks[1], view->offer_ticks[2],
                      view->offer_lots[0], view->offer_lots[1], view->offer_lots[2],
                      view->offer_count[0], view->offer_count[1], view->offer_count[2],
                      view->created);
}

DBR_API size_t
dbr_json_err_len(int num, const char* msg)
{
    enum {
        ERR_SIZE =
        sizeof("{\"num\":,"
               "\"msg\":\"\"}") - 1
    };

    return ERR_SIZE
        + dbr_int_len(num)
        + strnlen(msg, DBR_ERRMSG_MAX);
}

DBR_API char*
dbr_json_write_err(char* buf, int num, const char* msg)
{
    static const char ERR_FORMAT[] =
        "{\"num\":\"%d\","
        "\"msg\":\"%s\"}";

    return dbr_format(buf, ERR_FORMAT,
                      num,
                      DBR_ERRMSG_MAX, msg);
}
