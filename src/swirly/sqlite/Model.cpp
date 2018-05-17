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
#include "Model.hpp"

#include "Utility.hxx"

#include <swirly/fin/Asset.hpp>
#include <swirly/fin/Exec.hpp>
#include <swirly/fin/Instr.hpp>
#include <swirly/fin/Market.hpp>

#include <swirly/fin/Order.hpp>
#include <swirly/fin/Posn.hpp>

#include <swirly/util/Config.hpp>

namespace swirly {
inline namespace sqlite {
using namespace std;
namespace {

constexpr auto SelectAssetSql = //
    "SELECT id, symbol, display, type_id FROM asset_t"sv;

constexpr auto SelectInstrSql =                                               //
    "SELECT id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom," //
    " tick_numer, tick_denom, pip_dp, min_lots, max_lots FROM instr_v"sv;

constexpr auto SelectMarketSql =                                                   //
    "SELECT id, instr, settl_day, state, last_lots, last_ticks, last_time, max_id" //
    " FROM market_v"sv;

constexpr auto SelectOrderSql =                                                            //
    "SELECT accnt, market_id, instr, settl_day, id, ref, state_id, side_id, lots, ticks,"  //
    " resd_lots, exec_lots, exec_cost, last_lots, last_ticks, min_lots, created, modified" //
    " FROM order_t WHERE resd_lots > 0;"sv;

constexpr auto SelectExecSql =                                                               //
    "SELECT accnt, market_id, instr, settl_day, id, order_id, ref, state_id, side_id, lots," //
    " ticks, resd_lots, exec_lots, exec_cost, last_lots, last_ticks, min_lots, match_id,"    //
    " liqInd_id, cpty, created"                                                              //
    " FROM exec_t WHERE created > ? ORDER BY seq_id DESC;"sv;

constexpr auto SelectTradeSql =                                                               //
    "SELECT accnt, market_id, instr, settl_day, id, order_id, ref, side_id, lots, ticks,"     //
    " resd_lots, exec_lots, exec_cost, last_lots, last_ticks, min_lots, match_id, liqInd_id," //
    " cpty, created"                                                                          //
    " FROM exec_t WHERE state_id = 4 AND archive IS NULL;"sv;

constexpr auto SelectPosnSql = //
    "SELECT accnt, market_id, instr, settl_day, side_id, lots, cost FROM posn_v;"sv;

} // namespace

SqlModel::SqlModel(const Config& config)
: db_{openDb(config.get("sqlite_model", "swirly.db"), SQLITE_OPEN_READONLY, config)}
{
}

SqlModel::~SqlModel() = default;

SqlModel::SqlModel(SqlModel&&) = default;

SqlModel& SqlModel::operator=(SqlModel&&) = default;

void SqlModel::doReadAsset(const ModelCallback<AssetPtr>& cb) const
{
    enum {
        Id,      //
        Symbol,  //
        Display, //
        TypeId   //
    };

    StmtPtr stmt{prepare(*db_, SelectAssetSql)};
    while (step(*stmt)) {
        cb(Asset::make(column<Id32>(*stmt, Id),             //
                       column<string_view>(*stmt, Symbol),  //
                       column<string_view>(*stmt, Display), //
                       column<AssetType>(*stmt, TypeId)));
    }
}

void SqlModel::doReadInstr(const ModelCallback<InstrPtr>& cb) const
{
    enum {
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

    StmtPtr stmt{prepare(*db_, SelectInstrSql)};
    while (step(*stmt)) {
        cb(Instr::make(column<Id32>(*stmt, Id),               //
                       column<string_view>(*stmt, Symbol),    //
                       column<string_view>(*stmt, Display),   //
                       column<string_view>(*stmt, BaseAsset), //
                       column<string_view>(*stmt, TermCcy),   //
                       column<int>(*stmt, LotNumer),          //
                       column<int>(*stmt, LotDenom),          //
                       column<int>(*stmt, TickNumer),         //
                       column<int>(*stmt, TickDenom),         //
                       column<int>(*stmt, PipDp),             //
                       column<Lots>(*stmt, MinLots),          //
                       column<Lots>(*stmt, MaxLots)));
    }
}

void SqlModel::doReadMarket(const ModelCallback<MarketPtr>& cb) const
{
    enum {         //
        Id,        //
        Instr,     //
        SettlDay,  //
        State,     //
        LastLots,  //
        LastTicks, //
        LastTime,  //
        MaxId      //
    };

    StmtPtr stmt{prepare(*db_, SelectMarketSql)};
    while (step(*stmt)) {
        cb(Market::make(column<Id64>(*stmt, Id),           //
                        column<string_view>(*stmt, Instr), //
                        column<JDay>(*stmt, SettlDay),     //
                        column<MarketState>(*stmt, State), //
                        column<Lots>(*stmt, LastLots),     //
                        column<Ticks>(*stmt, LastTicks),   //
                        column<Time>(*stmt, LastTime),     //
                        column<Id64>(*stmt, MaxId)));
    }
}

void SqlModel::doReadOrder(const ModelCallback<OrderPtr>& cb) const
{
    enum {         //
        Accnt,     //
        MarketId,  //
        Instr,     //
        SettlDay,  //
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
        MinLots,   //
        Created,   //
        Modified   //
    };

    StmtPtr stmt{prepare(*db_, SelectOrderSql)};
    while (step(*stmt)) {
        cb(Order::make(column<string_view>(*stmt, Accnt),       //
                       column<Id64>(*stmt, MarketId),           //
                       column<string_view>(*stmt, Instr),       //
                       column<JDay>(*stmt, SettlDay),           //
                       column<Id64>(*stmt, Id),                 //
                       column<string_view>(*stmt, Ref),         //
                       column<swirly::State>(*stmt, State),     //
                       column<swirly::Side>(*stmt, Side),       //
                       column<swirly::Lots>(*stmt, Lots),       //
                       column<swirly::Ticks>(*stmt, Ticks),     //
                       column<swirly::Lots>(*stmt, ResdLots),   //
                       column<swirly::Lots>(*stmt, ExecLots),   //
                       column<swirly::Cost>(*stmt, ExecCost),   //
                       column<swirly::Lots>(*stmt, LastLots),   //
                       column<swirly::Ticks>(*stmt, LastTicks), //
                       column<swirly::Lots>(*stmt, MinLots),    //
                       column<Time>(*stmt, Created),            //
                       column<Time>(*stmt, Modified)));
    }
}

void SqlModel::doReadExec(Time since, const ModelCallback<ExecPtr>& cb) const
{
    enum {         //
        Accnt,     //
        MarketId,  //
        Instr,     //
        SettlDay,  //
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
        LiqInd,    //
        Cpty,      //
        Created    //
    };

    StmtPtr stmt{prepare(*db_, SelectExecSql)};
    ScopedBind bind{*stmt};
    bind(since);
    while (step(*stmt)) {
        cb(Exec::make(column<string_view>(*stmt, Accnt),       //
                      column<Id64>(*stmt, MarketId),           //
                      column<string_view>(*stmt, Instr),       //
                      column<JDay>(*stmt, SettlDay),           //
                      column<Id64>(*stmt, Id),                 //
                      column<Id64>(*stmt, OrderId),            //
                      column<string_view>(*stmt, Ref),         //
                      column<swirly::State>(*stmt, State),     //
                      column<swirly::Side>(*stmt, Side),       //
                      column<swirly::Lots>(*stmt, Lots),       //
                      column<swirly::Ticks>(*stmt, Ticks),     //
                      column<swirly::Lots>(*stmt, ResdLots),   //
                      column<swirly::Lots>(*stmt, ExecLots),   //
                      column<swirly::Cost>(*stmt, ExecCost),   //
                      column<swirly::Lots>(*stmt, LastLots),   //
                      column<swirly::Ticks>(*stmt, LastTicks), //
                      column<swirly::Lots>(*stmt, MinLots),    //
                      column<Id64>(*stmt, MatchId),            //
                      column<swirly::LiqInd>(*stmt, LiqInd),   //
                      column<string_view>(*stmt, Cpty),        //
                      column<Time>(*stmt, Created)));
    }
}

void SqlModel::doReadTrade(const ModelCallback<ExecPtr>& cb) const
{
    enum {         //
        Accnt,     //
        MarketId,  //
        Instr,     //
        SettlDay,  //
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
        LiqInd,    //
        Cpty,      //
        Created    //
    };

    StmtPtr stmt{prepare(*db_, SelectTradeSql)};
    while (step(*stmt)) {
        cb(Exec::make(column<string_view>(*stmt, Accnt),       //
                      column<Id64>(*stmt, MarketId),           //
                      column<string_view>(*stmt, Instr),       //
                      column<JDay>(*stmt, SettlDay),           //
                      column<Id64>(*stmt, Id),                 //
                      column<Id64>(*stmt, OrderId),            //
                      column<string_view>(*stmt, Ref),         //
                      State::Trade,                            //
                      column<swirly::Side>(*stmt, Side),       //
                      column<swirly::Lots>(*stmt, Lots),       //
                      column<swirly::Ticks>(*stmt, Ticks),     //
                      column<swirly::Lots>(*stmt, ResdLots),   //
                      column<swirly::Lots>(*stmt, ExecLots),   //
                      column<swirly::Cost>(*stmt, ExecCost),   //
                      column<swirly::Lots>(*stmt, LastLots),   //
                      column<swirly::Ticks>(*stmt, LastTicks), //
                      column<swirly::Lots>(*stmt, MinLots),    //
                      column<Id64>(*stmt, MatchId),            //
                      column<swirly::LiqInd>(*stmt, LiqInd),   //
                      column<string_view>(*stmt, Cpty),        //
                      column<Time>(*stmt, Created)));
    }
}

void SqlModel::doReadPosn(JDay busDay, const ModelCallback<PosnPtr>& cb) const
{
    enum {        //
        Accnt,    //
        MarketId, //
        Instr,    //
        SettlDay, //
        Side,     //
        Lots,     //
        Cost      //
    };

    PosnSet ps;
    PosnSet::Iterator it;

    StmtPtr stmt{prepare(*db_, SelectPosnSql)};
    while (step(*stmt)) {
        const auto accnt = column<string_view>(*stmt, Accnt);
        auto marketId = column<Id64>(*stmt, MarketId);
        const auto instr = column<string_view>(*stmt, Instr);
        auto settlDay = column<JDay>(*stmt, SettlDay);

        // FIXME: review when end of day is implemented.
        if (settlDay != 0_jd && settlDay <= busDay) {
            marketId &= Id64{~0xffff};
            settlDay = 0_jd;
        }

        bool found;
        tie(it, found) = ps.findHint(accnt, marketId);
        if (!found) {
            it = ps.insertHint(it, Posn::make(accnt, marketId, instr, settlDay));
        }

        const auto side = column<swirly::Side>(*stmt, Side);
        const auto lots = column<swirly::Lots>(*stmt, Lots);
        const auto cost = column<swirly::Cost>(*stmt, Cost);
        if (side == swirly::Side::Buy) {
            it->addBuy(lots, cost);
        } else {
            it->addSell(lots, cost);
        }
    }

    for (it = ps.begin(); it != ps.end();) {
        cb(ps.remove(it++));
    }
}

} // namespace sqlite
} // namespace swirly
