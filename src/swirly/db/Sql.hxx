/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#ifndef SWIRLY_DB_SQL_HXX
#define SWIRLY_DB_SQL_HXX

#include <string_view>

namespace swirly {
using namespace std::literals::string_view_literals;
inline namespace db {

namespace asset {
enum : int {
    Id,      //
    Symbol,  //
    Display, //
    TypeId   //
};

constexpr auto SelectSql = //
    "SELECT id, symbol, display, type_id FROM asset_t"sv;
} // namespace asset

namespace instr {
enum : int {
    Id,        //
    Symbol,    //
    Display,   //
    BaseAsset, //
    TermCcy,   //
    LotNumer,  //
    LotDenom,  //
    TickNumer, //
    TickDenom, //
    PipDp,     //
    MinLots,   //
    MaxLots    //
};

constexpr auto SelectSql =                                                    //
    "SELECT id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom," //
    " tick_numer, tick_denom, pip_dp, min_lots, max_lots FROM instr_v"sv;
} // namespace instr

namespace market {
enum : int {   //
    Id,        //
    Instr,     //
    SettlDay,  //
    State,     //
    LastLots,  //
    LastTicks, //
    LastTime,  //
    MaxId      //
};

constexpr auto SelectSql =                                                         //
    "SELECT id, instr, settl_day, state, last_time, last_lots, last_ticks, max_id" //
    " FROM market_v"sv;

constexpr auto InsertSql =                               //
    "INSERT INTO market_t (id, instr, settl_day, state)" //
    " VALUES (?, ?, ?, ?)"sv;

constexpr auto UpdateSql =          //
    "UPDATE market_t SET state = ?" //
    " WHERE id = ?"sv;
} // namespace market

namespace order {
enum : int {   //
    Created,   //
    Modified,  //
    Accnt,     //
    MarketId,  //
    Id,        //
    Ref,       //
    State,     //
    Side,      //
    Lots,      //
    Ticks,     //
    ResdLots,  //
    ExecLots,  //
    ExecCost,  //
    LastLots,  //
    LastTicks, //
    MinLots    //
};

constexpr auto SelectSql
    = "SELECT"                                                                   //
      " created, modified, accnt, market_id, id, ref, state_id, side_id, lots,"  //
      " ticks, resd_lots, exec_lots, exec_cost, last_lots, last_ticks, min_lots" //
      " FROM order_t WHERE resd_lots > 0;"sv;
} // namespace order

namespace exec {
enum : int {   //
    Created,   //
    Accnt,     //
    MarketId,  //
    Id,        //
    OrderId,   //
    Ref,       //
    State,     //
    Side,      //
    Lots,      //
    Ticks,     //
    ResdLots,  //
    ExecLots,  //
    ExecCost,  //
    LastLots,  //
    LastTicks, //
    MinLots,   //
    MatchId,   //
    PosnLots,  //
    PosnCost,  //
    LiqInd,    //
    Cpty       //
};

constexpr auto SelectSql
    = "SELECT"                                                                    //
      " created, accnt, market_id, id, order_id, ref, state_id, side_id, lots,"   //
      " ticks, resd_lots, exec_lots, exec_cost, last_lots, last_ticks, min_lots," //
      " match_id, posn_lots, posn_cost, liq_ind_id, cpty"                         //
      " FROM exec_t WHERE created > ? ORDER BY seq_id DESC;"sv;

constexpr auto InsertSql
    = "INSERT INTO exec_t ("                                                      //
      "created, accnt, market_id, id, order_id, ref, state_id, side_id, lots,"    //
      " ticks, resd_lots, exec_lots, exec_cost, last_lots, last_ticks, min_lots," //
      " match_id, posn_lots, posn_cost, liq_ind_id, cpty"                         //
      ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"sv;

constexpr auto UpdateSql =          //
    "UPDATE exec_t SET archive = ?" //
    " WHERE market_id = ? AND id = ?"sv;
} // namespace exec

namespace trade {
enum : int {   //
    Created,   //
    Accnt,     //
    MarketId,  //
    Id,        //
    OrderId,   //
    Ref,       //
    Side,      //
    Lots,      //
    Ticks,     //
    ResdLots,  //
    ExecLots,  //
    ExecCost,  //
    LastLots,  //
    LastTicks, //
    MinLots,   //
    MatchId,   //
    PosnLots,  //
    PosnCost,  //
    LiqInd,    //
    Cpty       //
};

constexpr auto SelectSql
    = "SELECT"                                                                       //
      " created, accnt, market_id, id, order_id, ref, side_id, lots, ticks,"         //
      " resd_lots, exec_lots, exec_cost, last_lots, last_ticks, min_lots, match_id," //
      " posn_lots, posn_cost, liq_ind_id, cpty"                                      //
      " FROM exec_t WHERE state_id = 4 AND archive IS NULL;"sv;
} // namespace trade

namespace posn {
enum : int {  //
    Accnt,    //
    MarketId, //
    Side,     //
    Lots,     //
    Cost      //
};

constexpr auto SelectSql = //
    "SELECT accnt, market_id, side_id, lots, cost FROM posn_v;"sv;
} // namespace posn

} // namespace db
} // namespace swirly

#endif // SWIRLY_DB_SQL_HXX
