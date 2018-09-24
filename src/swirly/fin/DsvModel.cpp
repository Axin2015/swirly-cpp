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
#include "DsvModel.hpp"

#include "Asset.hpp"
#include "Exec.hpp"
#include "Instr.hpp"
#include "Market.hpp"
#include "Posn.hpp"

#include <swirly/util/Tokeniser.hpp>

#include <fstream>

namespace swirly {
inline namespace fin {
using namespace std;

DsvModel::DsvModel() noexcept = default;

DsvModel::~DsvModel() = default;

void DsvModel::do_read_asset(const ModelCallback<AssetPtr>& cb) const
{
    enum {
        Id,      //
        Symbol,  //
        Display, //
        TypeId,  //
        ColCount //
    };

    ifstream is{"asset.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<ColCount> row;
        split(line, "\t"sv, row);
        const auto id = from_string<Id32>(row[Id]);
        const auto symbol = from_string<string_view>(row[Symbol]);
        const auto display = from_string<string_view>(row[Display]);
        const auto type = from_string<AssetType>(row[TypeId]);
        cb(Asset::make(id, symbol, display, type));
    }
}

void DsvModel::do_read_instr(const ModelCallback<InstrPtr>& cb) const
{
    enum {
        Id,        //
        Symbol,    //
        Display,   //
        BaseAsset, //
        TermCcy,   //
        TickNumer, //
        TickDenom, //
        LotNumer,  //
        LotDenom,  //
        PipDp,     //
        MinLots,   //
        MaxLots,   //
        ColCount   //
    };

    ifstream is{"instr.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<ColCount> row;
        split(line, "\t"sv, row);
        const auto id = from_string<Id32>(row[Id]);
        const auto symbol = from_string<string_view>(row[Symbol]);
        const auto display = from_string<string_view>(row[Display]);
        const auto base_asset = from_string<swirly::Symbol>(row[BaseAsset]);
        const auto term_ccy = from_string<swirly::Symbol>(row[TermCcy]);
        const auto lot_numer = from_string<int>(row[LotNumer]);
        const auto lot_denom = from_string<int>(row[LotDenom]);
        const auto tick_numer = from_string<int>(row[TickNumer]);
        const auto tick_denom = from_string<int>(row[TickDenom]);
        const auto pip_dp = from_string<int>(row[PipDp]);
        const auto min_lots = from_string<Lots>(row[MinLots]);
        const auto max_lots = from_string<Lots>(row[MaxLots]);
        cb(Instr::make(id, symbol, display, base_asset, term_ccy, lot_numer, lot_denom, tick_numer,
                       tick_denom, pip_dp, min_lots, max_lots));
    }
}

void DsvModel::do_read_market(const ModelCallback<MarketPtr>& cb) const
{
    enum {         //
        Id,        //
        Instr,     //
        SettlDay,  //
        State,     //
        LastLots,  //
        LastTicks, //
        LastTime,  //
        MaxId,     //
        ColCount   //
    };

    ifstream is{"market.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<ColCount> row;
        split(line, "\t"sv, row);
        const auto id = from_string<Id64>(row[Id]);
        const auto instr = from_string<swirly::Symbol>(row[Instr]);
        const auto settl_day = from_string<JDay>(row[SettlDay]);
        const auto state = from_string<MarketState>(row[State]);
        const auto last_time = from_string<WallTime>(row[LastTime]);
        const auto last_lots = from_string<Lots>(row[LastLots]);
        const auto last_ticks = from_string<Ticks>(row[LastTicks]);
        const auto max_id = from_string<Id64>(row[MaxId]);
        cb(Market::make(id, instr, settl_day, state, last_time, last_lots, last_ticks, max_id));
    }
}

void DsvModel::do_read_order(const ModelCallback<OrderPtr>& cb) const
{
    enum {         //
        Created,   //
        Modified,  //
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
        ColCount   //
    };

    ifstream is{"order.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<ColCount> row;
        split(line, "\t"sv, row);
        const auto created = from_string<WallTime>(row[Created]);
        const auto modified = from_string<WallTime>(row[Modified]);
        const auto accnt = from_string<Symbol>(row[Accnt]);
        const auto market_id = from_string<Id64>(row[MarketId]);
        const auto instr = from_string<Symbol>(row[Instr]);
        const auto settl_day = from_string<JDay>(row[SettlDay]);
        const auto id = from_string<Id64>(row[Id]);
        const auto ref = from_string<string_view>(row[Ref]);
        const auto state = from_string<swirly::State>(row[State]);
        const auto side = from_string<swirly::Side>(row[Side]);
        const auto lots = from_string<swirly::Lots>(row[Lots]);
        const auto ticks = from_string<swirly::Ticks>(row[Ticks]);
        const auto resd_lots = from_string<swirly::Lots>(row[ResdLots]);
        const auto exec_lots = from_string<swirly::Lots>(row[ExecLots]);
        const auto exec_cost = from_string<Cost>(row[ExecCost]);
        const auto last_lots = from_string<swirly::Lots>(row[LastLots]);
        const auto last_ticks = from_string<swirly::Ticks>(row[LastTicks]);
        const auto min_lots = from_string<swirly::Lots>(row[MinLots]);
        cb(Order::make(created, modified, accnt, market_id, instr, settl_day, id, ref, state, side,
                       lots, ticks, resd_lots, exec_lots, exec_cost, last_lots, last_ticks,
                       min_lots));
    }
}

void DsvModel::do_read_exec(WallTime since, const ModelCallback<ExecPtr>& cb) const
{
    enum {         //
        Created,   //
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
        PosnLots,  //
        PosnCost,  //
        LiqInd,    //
        Cpty,      //
        ColCount   //
    };

    ifstream is{"exec.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<ColCount> row;
        split(line, "\t"sv, row);
        const auto created = from_string<WallTime>(row[Created]);
        const auto accnt = from_string<swirly::Symbol>(row[Accnt]);
        const auto market_id = from_string<Id64>(row[MarketId]);
        const auto instr = from_string<Symbol>(row[Instr]);
        const auto settl_day = from_string<JDay>(row[SettlDay]);
        const auto id = from_string<Id64>(row[Id]);
        const auto order_id = from_string<Id64>(row[OrderId]);
        const auto ref = from_string<string_view>(row[Ref]);
        const auto state = from_string<swirly::State>(row[State]);
        const auto side = from_string<swirly::Side>(row[Side]);
        const auto lots = from_string<swirly::Lots>(row[Lots]);
        const auto ticks = from_string<swirly::Ticks>(row[Ticks]);
        const auto resd_lots = from_string<swirly::Lots>(row[ResdLots]);
        const auto exec_lots = from_string<swirly::Lots>(row[ExecLots]);
        const auto exec_cost = from_string<Cost>(row[ExecCost]);
        const auto last_lots = from_string<swirly::Lots>(row[LastLots]);
        const auto last_ticks = from_string<swirly::Ticks>(row[LastTicks]);
        const auto min_lots = from_string<swirly::Lots>(row[MinLots]);
        const auto match_id = from_string<Id64>(row[MatchId]);
        const auto posn_lots = from_string<swirly::Lots>(row[PosnLots]);
        const auto posn_cost = from_string<Cost>(row[PosnCost]);
        const auto liq_ind = from_string<swirly::LiqInd>(row[LiqInd]);
        const auto cpty = from_string<Symbol>(row[Cpty]);
        cb(Exec::make(created, accnt, market_id, instr, settl_day, id, order_id, ref, state, side,
                      lots, ticks, resd_lots, exec_lots, exec_cost, last_lots, last_ticks, min_lots,
                      match_id, posn_lots, posn_cost, liq_ind, cpty));
    }
}

void DsvModel::do_read_trade(const ModelCallback<ExecPtr>& cb) const
{
    enum {         //
        Created,   //
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
        PosnLots,  //
        PosnCost,  //
        LiqInd,    //
        Cpty,      //
        ColCount   //
    };

    ifstream is{"trade.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<ColCount> row;
        split(line, "\t"sv, row);
        const auto created = from_string<WallTime>(row[Created]);
        const auto accnt = from_string<swirly::Symbol>(row[Accnt]);
        const auto market_id = from_string<Id64>(row[MarketId]);
        const auto instr = from_string<Symbol>(row[Instr]);
        const auto settl_day = from_string<JDay>(row[SettlDay]);
        const auto id = from_string<Id64>(row[Id]);
        const auto order_id = from_string<Id64>(row[OrderId]);
        const auto ref = from_string<string_view>(row[Ref]);
        const auto side = from_string<swirly::Side>(row[Side]);
        const auto lots = from_string<swirly::Lots>(row[Lots]);
        const auto ticks = from_string<swirly::Ticks>(row[Ticks]);
        const auto resd_lots = from_string<swirly::Lots>(row[ResdLots]);
        const auto exec_lots = from_string<swirly::Lots>(row[ExecLots]);
        const auto exec_cost = from_string<Cost>(row[ExecCost]);
        const auto last_lots = from_string<swirly::Lots>(row[LastLots]);
        const auto last_ticks = from_string<swirly::Ticks>(row[LastTicks]);
        const auto min_lots = from_string<swirly::Lots>(row[MinLots]);
        const auto match_id = from_string<Id64>(row[MatchId]);
        const auto posn_lots = from_string<swirly::Lots>(row[PosnLots]);
        const auto posn_cost = from_string<Cost>(row[PosnCost]);
        const auto liq_ind = from_string<swirly::LiqInd>(row[LiqInd]);
        const auto cpty = from_string<Symbol>(row[Cpty]);
        cb(Exec::make(created, accnt, market_id, instr, settl_day, id, order_id, ref, State::Trade,
                      side, lots, ticks, resd_lots, exec_lots, exec_cost, last_lots, last_ticks,
                      min_lots, match_id, posn_lots, posn_cost, liq_ind, cpty));
    }
}

void DsvModel::do_read_posn(JDay bus_day, const ModelCallback<PosnPtr>& cb) const
{
    enum {        //
        Accnt,    //
        MarketId, //
        Instr,    //
        SettlDay, //
        BuyLots,  //
        BuyCost,  //
        SellLots, //
        SellCost, //
        ColCount  //
    };

    ifstream is{"posn.txt"};
    string line;

    // Discard header.
    if (!getline(is, line)) {
        return;
    }

    while (getline(is, line)) {
        Row<ColCount> row;
        split(line, "\t"sv, row);
        const auto accnt = from_string<Symbol>(row[Accnt]);
        const auto market_id = from_string<Id64>(row[MarketId]);
        const auto instr = from_string<Symbol>(row[Instr]);
        const auto settl_day = from_string<JDay>(row[SettlDay]);
        const auto buy_lots = from_string<Lots>(row[BuyLots]);
        const auto buy_cost = from_string<Cost>(row[BuyCost]);
        const auto sell_lots = from_string<Lots>(row[SellLots]);
        const auto sell_cost = from_string<Cost>(row[SellCost]);
        cb(Posn::make(accnt, market_id, instr, settl_day, buy_lots, buy_cost, sell_lots,
                      sell_cost));
    }
}

} // namespace fin
} // namespace swirly
