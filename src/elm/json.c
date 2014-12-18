/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/elm/json.h>

#include "format.h"

#include <sc/elm/types.h>

#include <sc/ash/util.h>

#include <assert.h>
#include <string.h>

SC_API size_t
sc_json_accnt_len(const struct ScRec* rec)
{
    assert(rec->type == SC_ENTITY_ACCNT);
    enum {
        ACCNT_SIZE =
        sizeof("{\"mnem\":\"\","
               "\"display\":\"\","
               "\"email\":\"\"}") - 1
    };

    return ACCNT_SIZE
        + strnlen(rec->mnem, SC_MNEM_MAX)
        + strnlen(rec->display, SC_DISPLAY_MAX)
        + strnlen(rec->accnt.email, SC_EMAIL_MAX);
}

SC_API char*
sc_json_write_accnt(char* buf, const struct ScRec* rec)
{
    assert(rec->type == SC_ENTITY_ACCNT);
    static const char ACCNT_FORMAT[] =
        "{\"mnem\":\"%m\","
        "\"display\":\"%s\","
        "\"email\":\"%s\"}";

    return sc_format(buf, ACCNT_FORMAT,
                      rec->mnem,
                      SC_DISPLAY_MAX, rec->display,
                      SC_EMAIL_MAX, rec->accnt.email);
}

SC_API size_t
sc_json_contr_len(const struct ScRec* rec)
{
    assert(rec->type == SC_ENTITY_CONTR);
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
        + strnlen(rec->mnem, SC_MNEM_MAX)
        + strnlen(rec->display, SC_DISPLAY_MAX)
        + strnlen(rec->contr.asset_type, SC_MNEM_MAX)
        + strnlen(rec->contr.asset, SC_MNEM_MAX)
        + strnlen(rec->contr.ccy, SC_MNEM_MAX)
        + sc_int_len(rec->contr.tick_numer)
        + sc_int_len(rec->contr.tick_denom)
        + sc_int_len(rec->contr.lot_numer)
        + sc_int_len(rec->contr.lot_denom)
        + sc_int_len(rec->contr.price_dp)
        + sc_int_len(rec->contr.pip_dp)
        + sc_int_len(rec->contr.qty_dp)
        + sc_long_len(rec->contr.min_lots)
        + sc_long_len(rec->contr.max_lots);
}

SC_API char*
sc_json_write_contr(char* buf, const struct ScRec* rec)
{
    assert(rec->type == SC_ENTITY_CONTR);
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

    return sc_format(buf, CONTR_FORMAT,
                      rec->mnem,
                      SC_DISPLAY_MAX, rec->display,
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

SC_API size_t
sc_json_perm_len(const struct ScPerm* perm)
{
    enum {
        PERM_SIZE =
        sizeof("{\"trader\":\"\","
               "\"giveup\":\"\"}") - 1
    };

    return PERM_SIZE
        + strnlen(perm->trader.rec->mnem, SC_MNEM_MAX)
        + strnlen(perm->giveup.rec->mnem, SC_MNEM_MAX);
}

SC_API char*
sc_json_write_perm(char* buf, const struct ScPerm* perm)
{
    static const char PERM_FORMAT[] =
        "{\"trader\":\"%m\","
        "\"giveup\":\"%m\"}";

    return sc_format(buf, PERM_FORMAT,
                      perm->trader.rec->mnem,
                      perm->giveup.rec->mnem);
}

SC_API size_t
sc_json_order_len(const struct ScOrder* order)
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
        + sc_long_len(order->id)
        + strnlen(order->i.trader.rec->mnem, SC_MNEM_MAX)
        + strnlen(order->i.giveup.rec->mnem, SC_MNEM_MAX)
        + strnlen(order->i.contr.rec->mnem, SC_MNEM_MAX)
        + 8
        + strnlen(order->i.ref, SC_REF_MAX)
        + elm_state_len(order->i.state)
        + elm_action_len(order->i.action)
        + sc_long_len(order->i.ticks)
        + sc_long_len(order->i.lots)
        + sc_long_len(order->i.resd)
        + sc_long_len(order->i.exec)
        + sc_long_len(order->i.last_ticks)
        + sc_long_len(order->i.last_lots)
        + sc_long_len(order->created)
        + sc_long_len(order->modified);
}

SC_API char*
sc_json_write_order(char* buf, const struct ScOrder* order)
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

    return sc_format(buf, ORDER_FORMAT,
                      order->id,
                      order->i.trader.rec->mnem,
                      order->i.giveup.rec->mnem,
                      order->i.contr.rec->mnem,
                      order->i.settl_day,
                      SC_REF_MAX, order->i.ref,
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

SC_API size_t
sc_json_exec_len(const struct ScExec* exec)
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
        + sc_long_len(exec->id)
        + sc_long_len(exec->order)
        + strnlen(exec->i.trader.rec->mnem, SC_MNEM_MAX)
        + strnlen(exec->i.giveup.rec->mnem, SC_MNEM_MAX)
        + strnlen(exec->i.contr.rec->mnem, SC_MNEM_MAX)
        + 8
        + strnlen(exec->i.ref, SC_REF_MAX)
        + elm_state_len(exec->i.state)
        + elm_action_len(exec->i.action)
        + sc_long_len(exec->i.ticks)
        + sc_long_len(exec->i.lots)
        + sc_long_len(exec->i.resd)
        + sc_long_len(exec->i.exec)
        + sc_long_len(exec->i.last_ticks)
        + sc_long_len(exec->i.last_lots)
        + sc_long_len(exec->match)
        + elm_role_len(exec->role)
        + strnlen(exec->cpty.rec->mnem, SC_MNEM_MAX)
        + sc_long_len(exec->created);
}

SC_API char*
sc_json_write_exec(char* buf, const struct ScExec* exec)
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

    return sc_format(buf, EXEC_FORMAT,
                      exec->id,
                      exec->order,
                      exec->i.trader.rec->mnem,
                      exec->i.giveup.rec->mnem,
                      exec->i.contr.rec->mnem,
                      exec->i.settl_day,
                      SC_REF_MAX, exec->i.ref,
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

SC_API size_t
sc_json_posn_len(const struct ScPosn* posn)
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
        + strnlen(posn->accnt.rec->mnem, SC_MNEM_MAX)
        + strnlen(posn->contr.rec->mnem, SC_MNEM_MAX)
        + 8
        + sc_long_len(posn->buy_licks)
        + sc_long_len(posn->buy_lots)
        + sc_long_len(posn->sell_licks)
        + sc_long_len(posn->sell_lots);
}

SC_API char*
sc_json_write_posn(char* buf, const struct ScPosn* posn)
{
    static const char POSN_FORMAT[] =
        "{\"accnt\":\"%m\","
        "\"contr\":\"%m\","
        "\"settl_date\":%j,"
        "\"buy_licks\":%l,"
        "\"buy_lots\":%l,"
        "\"sell_licks\":%l,"
        "\"sell_lots\":%l}";

    return sc_format(buf, POSN_FORMAT,
                      posn->accnt.rec->mnem,
                      posn->contr.rec->mnem,
                      posn->settl_day,
                      posn->buy_licks,
                      posn->buy_lots,
                      posn->sell_licks,
                      posn->sell_lots);
}

SC_API size_t
sc_json_view_len(const struct ScView* view)
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
        + strnlen(view->contr.rec->mnem, SC_MNEM_MAX)
        + 8
        + sc_long_len(view->created);

    for (size_t i = 0; i < SC_LEVEL_MAX; ++i) {
        len += sc_long_len(view->bid_ticks[i]);
        len += sc_long_len(view->bid_lots[i]);
        len += sc_long_len(view->bid_count[i]);
        len += sc_long_len(view->offer_ticks[i]);
        len += sc_long_len(view->offer_lots[i]);
        len += sc_long_len(view->offer_count[i]);
    }
    return len;
}

SC_API char*
sc_json_write_view(char* buf, const struct ScView* view)
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

    return sc_format(buf, VIEW_FORMAT,
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

SC_API size_t
sc_json_err_len(int num, const char* msg)
{
    enum {
        ERR_SIZE =
        sizeof("{\"num\":,"
               "\"msg\":\"\"}") - 1
    };

    return ERR_SIZE
        + sc_int_len(num)
        + strnlen(msg, SC_ERRMSG_MAX);
}

SC_API char*
sc_json_write_err(char* buf, int num, const char* msg)
{
    static const char ERR_FORMAT[] =
        "{\"num\":\"%d\","
        "\"msg\":\"%s\"}";

    return sc_format(buf, ERR_FORMAT,
                      num,
                      SC_ERRMSG_MAX, msg);
}
