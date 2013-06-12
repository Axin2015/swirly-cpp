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
#ifndef DBR_CORE_H
#define DBR_CORE_H

#include <dbr/model.h>

/**
 * @addtogroup Env
 * @{
 */

typedef struct DbrEnv_* DbrEnv;

/**
 * @brief Create environment.
 *
 * @param ctx Context.
 *
 * @param model Model.
 *
 * @return Handle to newly created environment or null on failure.
 */

DBR_API DbrEnv
dbr_env_create(DbrCtx ctx, DbrModel model);

// No-op if env is null.

DBR_API void
dbr_env_destroy(DbrEnv env);

/** @} */

/**
 * @addtogroup Rec
 * @{
 */

/**
 * @brief Returns first record of requested type.
 */

// Size is optional.

DBR_API struct DbrSlNode*
dbr_rec_entries(DbrEnv env, int type, size_t* size);

// Null if record does not exist.

DBR_API struct DbrSlNode*
dbr_rec_find_id(DbrEnv env, int type, DbrIden id);

// Null if record does not exist.
// This function compares DBR_MNEM_MAX characters of mnem at most.

DBR_API struct DbrSlNode*
dbr_rec_find_mnem(DbrEnv env, int type, const char* mnem);

DBR_API struct DbrSlNode*
dbr_rec_end(DbrEnv env);

/** @} */

/**
 * @addtogroup Market
 * @{
 */

DBR_API DbrMarket
dbr_market_lazy(DbrEnv env, struct DbrRec* mrec);

// Returns the market-id for state.

DBR_API DbrIden
dbr_market_id(DbrMarket market);

DBR_API DbrSide
dbr_market_bid_side(DbrMarket market);

DBR_API DbrSide
dbr_market_ask_side(DbrMarket market);

DBR_API struct DbrBest*
dbr_market_best(struct DbrRec* mrec, struct DbrBest* best);

/** @} */

/**
 * @addtogroup SideOrder
 * @{
 */

DBR_API struct DbrDlNode*
dbr_side_first_order(DbrSide side);

DBR_API struct DbrDlNode*
dbr_side_last_order(DbrSide side);

DBR_API struct DbrDlNode*
dbr_side_end_order(DbrSide side);

DBR_API DbrBool
dbr_side_empty_order(DbrSide side);

/** @} */

/**
 * @addtogroup SideLevel
 * @{
 */

DBR_API struct DbrRbNode*
dbr_side_find_level(DbrSide side, DbrTicks ticks);

DBR_API struct DbrRbNode*
dbr_side_first_level(DbrSide side);

DBR_API struct DbrRbNode*
dbr_side_last_level(DbrSide side);

DBR_API struct DbrRbNode*
dbr_side_end_level(DbrSide side);

DBR_API DbrBool
dbr_side_empty_level(DbrSide side);

/** @} */

/**
 * @addtogroup SideLast
 * @{
 */

DBR_API DbrTicks
dbr_side_last_ticks(DbrSide side);

DBR_API DbrLots
dbr_side_last_lots(DbrSide side);

DBR_API DbrMillis
dbr_side_last_time(DbrSide side);

/** @} */

/**
 * @addtogroup Trader
 * @{
 */

DBR_API DbrTrader
dbr_trader_lazy(DbrEnv env, struct DbrRec* trec);

// Returns the trader-id for state.

DBR_API DbrIden
dbr_trader_id(DbrTrader trader);

/** @} */

/**
 * @addtogroup TraderOrder
 * @{
 */

DBR_API struct DbrRbNode*
dbr_trader_find_order_id(DbrTrader trader, DbrIden id);

DBR_API struct DbrOrder*
dbr_trader_find_order_ref(DbrTrader trader, const char* ref);

DBR_API struct DbrRbNode*
dbr_trader_first_order(DbrTrader trader);

DBR_API struct DbrRbNode*
dbr_trader_last_order(DbrTrader trader);

DBR_API struct DbrRbNode*
dbr_trader_end_order(DbrTrader trader);

DBR_API DbrBool
dbr_trader_empty_order(DbrTrader trader);

/** @} */

/**
 * @addtogroup TraderSub
 * @{
 */

/**
 * @brief Subscribe to market.
 */

DBR_API DbrBool
dbr_trader_sub(DbrTrader trader, DbrMarket market);

/**
 * @brief Unsubscribe from market.
 */

DBR_API void
dbr_trader_unsub(DbrTrader trader, DbrIden mrid);

/** @} */

/**
 * @addtogroup TraderSess
 * @{
 */

DBR_API void
dbr_trader_set_sess(DbrTrader trader, DbrTraderSess sess);

DBR_API DbrTraderSess
dbr_trader_sess(DbrTrader trader);

/** @} */

/**
 * @addtogroup Accnt
 * @{
 */

DBR_API DbrAccnt
dbr_accnt_lazy(DbrEnv env, struct DbrRec* arec);

// Returns the account-id for state.

DBR_API DbrIden
dbr_accnt_id(DbrAccnt accnt);

/** @} */

/**
 * @addtogroup AccntTrade
 * @{
 */

DBR_API struct DbrRbNode*
dbr_accnt_find_trade_id(DbrAccnt accnt, DbrIden id);

DBR_API struct DbrRbNode*
dbr_accnt_first_trade(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_last_trade(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_end_trade(DbrAccnt accnt);

DBR_API DbrBool
dbr_accnt_empty_trade(DbrAccnt accnt);

/** @} */

/**
 * @addtogroup AccntPosn
 * @{
 */

DBR_API struct DbrRbNode*
dbr_accnt_find_posn_id(DbrAccnt accnt, DbrIden id);

DBR_API struct DbrRbNode*
dbr_accnt_first_posn(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_last_posn(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_end_posn(DbrAccnt accnt);

DBR_API DbrBool
dbr_accnt_empty_posn(DbrAccnt accnt);

/** @} */

/**
 * @addtogroup AccntSess
 * @{
 */

DBR_API void
dbr_accnt_set_sess(DbrAccnt accnt, DbrAccntSess sess);

DBR_API DbrAccntSess
dbr_accnt_sess(DbrAccnt accnt);

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

DBR_API struct DbrOrder*
dbr_exec_submit(DbrEnv env, struct DbrRec* trec, struct DbrRec* arec, const char* ref,
                struct DbrRec* mrec, int action, DbrTicks ticks, DbrLots lots, DbrLots min,
                DbrFlags flags, struct DbrTrans* trans);

// Assumes that order already belongs to this side.
// Reduced lots must not be:
// 1. less than executed lots;
// 2. less than min lots;
// 3. greater than original lots.

DBR_API struct DbrOrder*
dbr_exec_revise_id(DbrEnv env, DbrTrader trader, DbrIden id, DbrLots lots);

DBR_API struct DbrOrder*
dbr_exec_revise_ref(DbrEnv env, DbrTrader trader, const char* ref, DbrLots lots);

DBR_API struct DbrOrder*
dbr_exec_cancel_id(DbrEnv env, DbrTrader trader, DbrIden id);

DBR_API struct DbrOrder*
dbr_exec_cancel_ref(DbrEnv env, DbrTrader trader, const char* ref);

// Invalidates any pointers to the trade.

DBR_API DbrBool
dbr_exec_archive_order(DbrEnv env, DbrTrader trader, DbrIden id);

// Invalidates any pointers to the trade.

DBR_API DbrBool
dbr_exec_archive_trade(DbrEnv env, DbrAccnt accnt, DbrIden id);

/** @} */

/**
 * @addtogroup Trans
 * @{
 */

DBR_API void
dbr_exec_free_matches(DbrEnv env, struct DbrSlNode* first);

/** @} */

#endif // DBR_CORE_H
